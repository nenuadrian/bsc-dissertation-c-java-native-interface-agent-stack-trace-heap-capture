#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h> //inet_addr
#include <sys/types.h>
#include <time.h>
#include "miniz.c"
#include <dirent.h>
#include "server_communication.h"
#include <pwd.h>
#include "common_data_structures.h"
#include "exception_handling.h"
#include "xml_utils.h"
#include <openssl/md5.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#define max(a,b) ((a) < (b) ? (b) : (a))

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef linux
#include <pthread.h>
#endif

#define THREAD_APP_PING_TIMER 1
#define THREAD_EXCEPTION 2
#define THREAD_APP_START 3
#define APP_PING_TIMER_INTERVAL 120

//http://rosettacode.org/wiki/MD5#C
using namespace std;

int VERBOSE = 4;
char VERSION[6] = "1.0.0";
char * API_KEY;
pid_t PID;
char HOSTNAME[301];
static GlobalAgentData *gdata;
App app;

/***
 * THREAD WORKERS
 ***/

 void createThread(int type, void *arg);

 void * sendAppStartMessage(void * arg) {
   xmlDocPtr doc = NULL;
   xmlNodePtr root = NULL, node = NULL, app_root = NULL, e = NULL;

   doc = xmlNewDoc(BAD_CAST "1.0");
   root = xmlNewNode(NULL, BAD_CAST "msg");
   xmlDocSetRootElement(doc, root);
   app_root = xmlNewChild(root, NULL, BAD_CAST "app", NULL);

   e = xmlNewChild(app_root, NULL, BAD_CAST "event", NULL);
   xmlNodeAddContent(e, BAD_CAST "start");

   node = xmlNewChild(app_root, NULL, BAD_CAST "hostname", NULL);
   xmlNodeAddContent(node, BAD_CAST HOSTNAME);

   node = xmlNewChild(app_root, NULL, BAD_CAST "pid", NULL);
   char buffer[20];
   sprintf(buffer, "%d", PID);
   xmlNodeAddContent(node, BAD_CAST buffer);

   node = xmlNewChild(app_root, NULL, BAD_CAST "command", NULL);
   xmlNodeAddContent(node, BAD_CAST app.command);

   node = xmlNewChild(app_root, NULL, BAD_CAST "p_name", NULL);
   xmlNodeAddContent(node, BAD_CAST app.process_name);

   node = xmlNewChild(app_root, NULL, BAD_CAST "c_path", NULL);
   xmlNodeAddContent(node, BAD_CAST app.class_path);

   node = xmlNewChild(app_root, NULL, BAD_CAST "jvm", NULL);
   xmlNodeAddContent(node, BAD_CAST app.jvm);

   node = xmlNewChild(app_root, NULL, BAD_CAST "agentv", NULL);
   xmlNodeAddContent(node, BAD_CAST VERSION);

   xmlChar * mem = docToChar(doc);
   char message[xmlStrlen(mem) + 1];
   sprintf(message, "%s", mem);

   xmlFree(mem);
   xmlFreeDoc(doc);

   sc_sendDataToServer(message, (char*)"app-start");

   createThread(THREAD_APP_PING_TIMER, (void*)"");

   return NULL;
 }


 void * exHolderWorker(void * arg) {
   debug("Exception processing thread started");
   struct ExceptionHolder * exHolder = (struct ExceptionHolder *) arg;

   xmlChar * mem = sc_exHolderToXmlChar(exHolder);
  // free(arg);

   if (mem != NULL) {
     char message[xmlStrlen(mem) + 1];
     sprintf(message, "%s", mem);

     xmlFree(mem);
     sc_sendDataToServer(message, (char *) "exception");
   }

   free(exHolder);
   return NULL;
 }

 void * appPingWorker(void *arg) {
   xmlDocPtr doc = NULL;
   xmlNodePtr root = NULL, app = NULL, event = NULL;

   doc = xmlNewDoc(BAD_CAST "1.0");
   root = xmlNewNode(NULL, BAD_CAST "msg");
   xmlDocSetRootElement(doc, root);

   app = xmlNewChild(root, NULL, BAD_CAST "app", NULL);

   event = xmlNewChild(app, NULL, BAD_CAST "event", NULL);
   xmlNodeAddContent(event, BAD_CAST "ping");

   xmlChar * mem = docToChar(doc);
   char message[xmlStrlen(mem) + 1];
   sprintf(message, "%s", mem);

   xmlFree(mem);
   xmlFreeDoc(doc);

   while(true) {
     sleep(APP_PING_TIMER_INTERVAL);
     info("Pinging app to server");
     sc_sendDataToServer(message, (char*)"app-ping");
   }
 }

 void * threadHandler(void * arg) {
   struct ThreadHolder * threadHolder = (struct ThreadHolder *)arg;
   void * output;
   switch(threadHolder->type) {
     case THREAD_APP_PING_TIMER:
     output = appPingWorker(threadHolder->arg);
     break;
     case THREAD_EXCEPTION:
     output = exHolderWorker(threadHolder->arg);
     break;
     case THREAD_APP_START:
     output = sendAppStartMessage(threadHolder->arg);
     break;
     default:
     output = NULL;
   }
   //free(arg);
   return output;
 }

 void createThread(int type, void * arg) {
   struct ThreadHolder  * threadHolder = (struct ThreadHolder *)malloc(sizeof(struct ThreadHolder));
   threadHolder->type = type;
   threadHolder->arg = arg;

  #if defined _WIN32 || defined _WIN64
   HANDLE thread = CreateThread(NULL, 0, threadHandler, (void*)threadHolder, 0, NULL);
  #endif

  #if defined __APPLE__ || defined linux
   int stack_size;
   pthread_attr_t thr_attr;
   stack_size = 1048576 * 1;
   pthread_attr_init(&thr_attr);
   pthread_attr_setstacksize(&thr_attr, stack_size);
   pthread_t pth;
   pthread_create(&pth, &thr_attr, threadHandler,  (void*)threadHolder);
  #endif
 }

 char * readFileContents(char * path) {
   if( access( path, F_OK ) != -1 ) {
     FILE *f = fopen(path, "rb");
     fseek(f, 0, SEEK_END);
     long fsize = ftell(f);
     fseek(f, 0, SEEK_SET);

     char *string = (char*)malloc(fsize + 1);
     fread(string, fsize, 1, f);
     fclose(f);
     string[fsize] = '\0';
     return string;
   }
   return NULL;
 }

 void JNICALL callbackException(jvmtiEnv *jvmti_env, JNIEnv* env, jthread thr, jmethodID method, jlocation location, jobject exception, jmethodID catch_method, jlocation catch_location)
 {
   if (app.app_hash == NULL || exception == NULL) return;

   info("Exception");

   struct ExceptionHolder * exHolder = (struct ExceptionHolder *)malloc(sizeof(struct ExceptionHolder));
   int result = callbackException(exHolder, jvmti_env, env, thr, method, location, exception, catch_method, catch_location);

   if (!result) {
     createThread(THREAD_EXCEPTION, (void*)exHolder);
     sleep(20);
   }
 }


 void listdir(const char *name, int level) {
   DIR *dir;
   struct dirent *entry;

   if (!(dir = opendir(name)))
     return;
   if (!(entry = readdir(dir)))
     return;

   do {
     char path[1024];
     int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
     path[len] = 0;
     if (entry->d_type == DT_DIR) {
       if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
         continue;
       listdir(path, level + 1);
     }
     else {
       const char *filename = strrchr(entry->d_name, '.');
       if (strcasecmp(filename, ".java") == 0) {


         char *string = readFileContents(path);
         char message[10000];

             /*sprintf(message, (char*)"<source_file><filename><![CDATA[%s]]></filename><contents><![CDATA[%s]]></contents></source_file>",
               entry->d_name, string);
             sc_sendDataToServer(message, (char *) "source");*/
}
}

} while ((entry = readdir(dir)));
closedir(dir);
}

void handlingJars(char * s_Test_archive_filename) {
  int i;
  mz_bool status;
  size_t uncomp_size;
  mz_zip_archive zip_archive;
  void *p;

  //printf("miniz.c version: %s\n", MZ_VERSION);

  memset(&zip_archive, 0, sizeof(zip_archive));

  status = mz_zip_reader_init_file(&zip_archive, s_Test_archive_filename, 0);
  if (!status)
  {
    return ;
  }

// Get and print information about each file in the archive.
  for (i = 0; i < (int)mz_zip_reader_get_num_files(&zip_archive); i++)
  {
    mz_zip_archive_file_stat file_stat;
    if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
    {
      mz_zip_reader_end(&zip_archive);
      return ;
    }

  // recursive parsing?
    if (mz_zip_reader_is_file_a_directory(&zip_archive, i))
    {
      /*printf("mz_zip_reader_is_file_a_directory() didn't return the expected results!\n");
      mz_zip_reader_end(&zip_archive);
      return EXIT_FAILURE;*/
    }
    else {
      const char *extension = strrchr(file_stat.m_filename, '.');
      if (strcasecmp(extension, ".java") != 0 && strcasecmp(extension, ".scala") != 0 ) continue;

      printf("Filename: \"%s\", Comment: \"%s\", Uncompressed size: %u, Compressed size: %u, Is Dir: %u\n", file_stat.m_filename, file_stat.m_comment, (uint)file_stat.m_uncomp_size, (uint)file_stat.m_comp_size, mz_zip_reader_is_file_a_directory(&zip_archive, i));


     // sprintf(archive_filename, "Agent/Agent.java");
      printf("%s %s %zu", file_stat.m_filename, s_Test_archive_filename, uncomp_size);
      // Try to extract all the files to the heap.
      p = mz_zip_reader_extract_file_to_heap(&zip_archive, file_stat.m_filename, &uncomp_size, 0);
      if (!p)
      {
        printf("mz_zip_reader_extract_file_to_heap() failed!\n");
        mz_zip_reader_end(&zip_archive);
        return ;
      }


      //printf("Successfully extracted file \"%s\", size %u\n", archive_filename, (uint)uncomp_size);
      //printf("File data: \"%s\"\n", (const char *)p);

      mz_free(p);
    }
  }

// Close the archive, freeing any resources it was using
  mz_zip_reader_end(&zip_archive);
}

JNIEXPORT void Agent_OnUnload(JavaVM *vm) {
  info("Agent UNLOADED");
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL, app = NULL, event = NULL;

  doc = xmlNewDoc(BAD_CAST "1.0");
  root = xmlNewNode(NULL, BAD_CAST "msg");
  xmlDocSetRootElement(doc, root);

  app = xmlNewChild(root, NULL, BAD_CAST "app", NULL);

  event = xmlNewChild(app, NULL, BAD_CAST "event", NULL);
  xmlNodeAddContent(event, BAD_CAST "stop");

  xmlChar * mem = docToChar(doc);
  char message[xmlStrlen(mem) + 1];
  sprintf(message, "%s", mem);
  sc_sendDataToServer(message, (char*)"app-stop");

  xmlFree(mem);
  xmlFreeDoc(doc);

  server_communication_cleanup();
  xmlCleanupParser();
    //TODO kill all threads, cleanup completely, allow for agent kill, kill agent?
}



void processSystemProperties(jvmtiEnv* env){
  app.class_path = NULL;
  app.process_name = NULL;

  PID = getpid();
  gethostname(HOSTNAME, 500);
  app.process_name = getenv("_");
  env->GetSystemProperty("java.class.path", &app.class_path);
  env->GetSystemProperty("sun.java.command", &app.command);
  env->GetSystemProperty("java.vm.name", &app.jvm);

  char *debugLevel;
  env->GetSystemProperty("codevo.Verbose", &debugLevel);

  if (strcmp(debugLevel, "0") == 0) VERBOSE = 1;
  if (strcmp(debugLevel, "1") == 0) VERBOSE = 2;
  if (strcmp(debugLevel, "2") == 0) VERBOSE = 3;
  if (strcmp(debugLevel, "3") == 0) VERBOSE = 4;

    // TODO handle sources?
  return;
  if (app.class_path != NULL && strlen(app.class_path) > 0 ) {
    const char *filename = strrchr(app.class_path, '.');
    if (strcasecmp(filename, ".jar") == 0)  {
      printf("test: this is a jar %s", "");
      handlingJars(app.class_path);
    }
  }

  char *sources;
  env->GetSystemProperty("codevo.Sources", &sources);

  if (sources != NULL) {
    listdir(sources, 0);
  }
}


JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM* jvm, char *options, void *reserved) {
  return Agent_OnLoad(jvm, options, reserved);
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char *options, void *reserved) {
  jvmtiError jvmti_err;

  info("Agent [v%s] attached - [w/ args: %s] - initiating ", VERSION, options);
  info("Setting up JVMTI bits and pieces");

  char * pch;
  pch = strtok(options, ",");
  while (pch != NULL) {
    if (strstr(pch, "api_key") != NULL) {
      API_KEY = pch;
    } else if (strstr(pch, "app_hash") != NULL) {
      app.app_hash = pch;
    }
    pch = strtok(NULL, ",");
  }
  pch = strtok(app.app_hash, ">");
  app.app_hash = strtok(NULL, ">");
  pch = strtok(API_KEY, ">");
  API_KEY = strtok(NULL, ">");


  jvmtiEnv *jvmti = NULL;
  jint result = jvm->GetEnv((void **) &jvmti, JVMTI_VERSION_1_1);
  if (result != JNI_OK) { error(" Unable to access JVMTI!"); return JNI_OK; }

  processSystemProperties(jvmti);

  jvmtiCapabilities capa;
  (void)memset(&capa, 0, sizeof(jvmtiCapabilities));

  capa.can_signal_thread = 1;
  capa.can_get_owned_monitor_info = 1;
  capa.can_access_local_variables = 1;
  capa.can_generate_method_entry_events = 1;
  capa.can_signal_thread = 1;
  capa.can_get_line_numbers = 1;
  capa.can_get_source_file_name = 1;
  capa.can_get_owned_monitor_info = 1;
  capa.can_generate_exception_events = 1;
  jvmti_err =(jvmti)->AddCapabilities(&capa);

  gdata = (GlobalAgentData*) malloc(sizeof(GlobalAgentData));
  gdata->jvmti = jvmti;

  jvmtiEventCallbacks callbacks;
  (void)memset(&callbacks, 0, sizeof(callbacks));
  callbacks.Exception = &callbackException;

  jvmti_err =jvmti->SetEventCallbacks(&callbacks,(jint)sizeof(callbacks));
  if (result != JNI_OK) { error(" Unable to set callbacks"); return JNI_OK; }

  static JNIEnv *jnienv;
  (*jvm).GetEnv((void **)&jnienv, JNI_VERSION_1_2);

  info("Verifying codEvo settings");

  info("API KEY: %s", API_KEY);
  info("API Hash: %s", app.app_hash);

  server_communication_init();
  LIBXML_TEST_VERSION

  createThread(THREAD_APP_START, (void*)"");

  jvmti_err = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_EXCEPTION, (jthread)NULL);
  return JNI_OK;
}

int main ( int argc, char **argv ) { return 0; }
