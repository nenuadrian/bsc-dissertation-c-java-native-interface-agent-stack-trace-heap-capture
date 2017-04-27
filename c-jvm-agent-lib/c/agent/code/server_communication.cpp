#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include "common_data_structures.h"
#include "xml_utils.h"
#include <jansson.h>

extern char * API_KEY;
extern pid_t PID;
extern App app;

static void write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) {
  size_t index = data->size;
  size_t n = (size * nmemb);
  char* tmp;

  data->size += (size * nmemb);

  tmp = (char*)realloc(data->data, data->size + 1); /* +1 for '\0' */

  if(tmp) {
    data->data = tmp;
  } else {
    if(data->data) {
      free(data->data);
    }
    fprintf(stderr, "Failed to allocate memory.\n");
    return;
  }

  memcpy((data->data + index), ptr, n);
  data->data[data->size] = '\0';

  int finalSize = size * nmemb;
}

// TODO compress?
char * sc_sendDataToServer(char message[], char * type) {
  CURL *curl = curl_easy_init();
  long http_code = 0;
  struct url_data data;
//  char post_fields[200 + strlen(message)];

  data.size = 0;
  data.data = (char*)malloc(4096); /* reasonable size initial buffer */
  if(NULL == data.data) return NULL;
  data.data[0] = '\0';
  if(curl) {

  json_t *root = json_object();
  json_object_set_new( root, "api_key", json_string(API_KEY) );
  json_object_set_new( root, "app_hash", json_string(app.app_hash) );
  json_object_set_new( root, "log_type", json_string(type) );
  json_object_set_new( root, "message", json_string(message) );
  char * msg = json_dumps(root, 0);
  info("Post data %s", msg);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");

    info("Setup request");
  //  info("Post data %s", message);
  //  info("Post data %s", msg);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8081/log");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, msg);
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_fields));

    info("Post with data of size: %d", sizeof(msg));

    curl_easy_perform(curl);
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);
  }

  if (data.data != NULL) {
     //free(data.data);
     debug(data.data);
  }

  return data.data;
}

void server_communication_init() {
  curl_global_init(CURL_GLOBAL_ALL);
}

void server_communication_cleanup() {
  curl_global_cleanup();
}

static void sc_processVar(xmlNodePtr var, VarHolder varHolder) {
  char buffer[30];
  xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
  xmlNodePtr node, cdataPtr;

  if (varHolder.is_static) {
    sprintf(buffer, "%d", varHolder.is_static);
    xmlNewProp(var, BAD_CAST "static", BAD_CAST buffer);
  }

  if (varHolder.name != NULL) {
    node = xmlNewChild(var, NULL, BAD_CAST "n", NULL);
    xmlNodeAddContent(node, BAD_CAST varHolder.name);
  }

  if (varHolder.signature != NULL) {
    node = xmlNewChild(var, NULL, BAD_CAST "s", NULL);
    xmlNodeAddContent(node, BAD_CAST varHolder.signature);
  }

  if (varHolder.generic != NULL) {
    node = xmlNewChild(var, NULL, BAD_CAST "g", NULL);
    cdataPtr = xmlNewCDataBlock(doc, BAD_CAST varHolder.generic, strlen(varHolder.generic) + 1);
    xmlAddChild( node, cdataPtr );
  }

  if (varHolder.value != NULL) {
    node = xmlNewChild(var, NULL, BAD_CAST "v", NULL);
    cdataPtr = xmlNewCDataBlock(doc, BAD_CAST varHolder.value, strlen(varHolder.value) + 1);
    xmlAddChild( node, cdataPtr );
  }

  if (varHolder.array_length >= 0) {
    sprintf(buffer, "%d", varHolder.array_length);
    node = xmlNewChild(var, NULL, BAD_CAST "a_l", NULL);
    xmlNodeAddContent(node, BAD_CAST buffer);
  }
}

static void recursiveProcessingOfVarsAndTheirFields(struct ExceptionHolder * exHolder, int count, xmlNodePtr * vars, VarHolder * objVars, int processObjects) {
  if (objVars == NULL ) return;
  xmlNodePtr var, node;
  for (int j = 0; j < count; j++) {
    if (objVars[j].name == NULL) continue;

    var = xmlNewChild(*(vars), NULL, BAD_CAST "v", NULL);

    if (objVars[j].object_id != -1 ) {

      char *object_map_key = (char*)malloc( sizeof(char) * ( 40 + 1 ) );
      sprintf(object_map_key, "%d", objVars[j].object_id);

      xmlNewProp(var, BAD_CAST "id", BAD_CAST object_map_key);
      VarHolder * object_in_map;

      if (processObjects != 1) {
        int map_error = hashmap_get(exHolder->object_map, object_map_key, (void**)(&object_in_map));
        if (map_error == MAP_OK) {
          continue;
        } else if (map_error == MAP_MISSING) {
          error("NOT FOUND, ID: %s", object_map_key);
        }
      }
    }

    sc_processVar(var, objVars[j]);

    if (objVars[j].fields != NULL) {
      node = xmlNewChild(var, NULL, BAD_CAST "f", NULL);
      recursiveProcessingOfVarsAndTheirFields(exHolder, objVars[j].field_count, &node, objVars[j].fields, 0);
    }
  }
   //free(objVars);
}

static int object_map_iterator(char * key, any_t varHolderArg, any_t exHolderArg, any_t objectsArg) {
 // xmlNodePtr objectsNode = *((xmlNodePtr *) objects), object, node;
  struct VarHolder varHolder = *((struct VarHolder *) varHolderArg);
  struct ExceptionHolder * exHolder = (struct ExceptionHolder *) exHolderArg;
  struct VarHolder * varHolders = (struct VarHolder *)malloc(sizeof(struct VarHolder));
  varHolders[0] = varHolder;
  recursiveProcessingOfVarsAndTheirFields(exHolder, 1, (xmlNodePtr *) objectsArg, varHolders, 1);
  return 0;
}

xmlChar * sc_exHolderToXmlChar(struct ExceptionHolder * exHolder) {

  char buffer[30];
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL, exception = NULL, node = NULL, stacks = NULL, cdataPtr = NULL;


  doc = xmlNewDoc(BAD_CAST "1.0");
  root = xmlNewNode(NULL, BAD_CAST "msg");
  xmlDocSetRootElement(doc, root);

  exception = xmlNewChild(root, NULL, BAD_CAST "ex", NULL);

  if (exHolder->caught != 0) {
    xmlNodePtr caught = xmlNewChild(exception, NULL, BAD_CAST "caught", NULL);
    // line
    printf(buffer, "%d", exHolder->caught);
    xmlNewProp(caught, BAD_CAST "l", BAD_CAST buffer);
    // method
    if (exHolder->caught_in_method != NULL) {
      node = xmlNewChild(caught, NULL, BAD_CAST "m", NULL);
      cdataPtr = xmlNewCDataBlock( doc, BAD_CAST exHolder->caught_in_method, strlen(exHolder->caught_in_method) + 1);
      xmlAddChild( node, cdataPtr );
    }
    // class signature
    if (exHolder->catcher->signature != NULL) {
      node = xmlNewChild(caught, NULL, BAD_CAST "sig", NULL);
      cdataPtr = xmlNewCDataBlock( doc, BAD_CAST exHolder->catcher->signature, strlen(exHolder->catcher->signature) + 1);
      xmlAddChild( node, cdataPtr );
    }
    // class generic
    if (exHolder->catcher->generic != NULL) {
      node = xmlNewChild(caught, NULL, BAD_CAST "g", NULL);
      cdataPtr = xmlNewCDataBlock( doc, BAD_CAST exHolder->catcher->generic, strlen(exHolder->catcher->generic) + 1);
      xmlAddChild( node, cdataPtr );
    }
    // class source
    if (exHolder->catcher->source != NULL) {
      node = xmlNewChild(caught, NULL, BAD_CAST "s", NULL);
      cdataPtr = xmlNewCDataBlock( doc, BAD_CAST exHolder->catcher->source, strlen(exHolder->catcher->source) + 1);
      xmlAddChild( node, cdataPtr );
    }
  }

  node = xmlNewChild(exception, NULL, BAD_CAST "getMessage", NULL);
  xmlNodeAddContent(node, BAD_CAST exHolder->getMessage);

  //debug("OBJECTS HASHMAP SIZE: %d", hashmap_length(exHolder->object_map));
  node = xmlNewChild(exception, NULL, BAD_CAST "objects", NULL);
  hashmap_iterate(exHolder->object_map, &object_map_iterator, exHolder, &node);

  stacks = xmlNewChild(exception, NULL, BAD_CAST "sts", NULL);

  if (exHolder->stacks != NULL) {
    for (int i = 0; i < exHolder->stack_count; i++) {
      xmlNodePtr stack = NULL, node = NULL;
      stack = xmlNewChild(stacks, NULL, BAD_CAST "st", NULL);
      // line
      printf(buffer, "%d", exHolder->stacks[i].line);
      xmlNewProp(stack, BAD_CAST "l", BAD_CAST buffer);
      // method
      if (exHolder->stacks[i].method != NULL) {
        node = xmlNewChild(stack, NULL, BAD_CAST "m", NULL);
        cdataPtr = xmlNewCDataBlock( doc, BAD_CAST exHolder->stacks[i].method, strlen(exHolder->stacks[i].method) + 1);
        xmlAddChild( node, cdataPtr );
      }
      // class signature
      if (exHolder->stacks[i].classHolder.signature) {
        node = xmlNewChild(stack, NULL, BAD_CAST "sig", NULL);
        cdataPtr = xmlNewCDataBlock( doc, BAD_CAST exHolder->stacks[i].classHolder.signature, strlen(exHolder->stacks[i].classHolder.signature) + 1);
        xmlAddChild( node, cdataPtr );
      }
      // class generic
      if (exHolder->stacks[i].classHolder.generic != NULL) {
        node = xmlNewChild(stack, NULL, BAD_CAST "g", NULL);
        cdataPtr = xmlNewCDataBlock( doc, BAD_CAST exHolder->stacks[i].classHolder.generic, strlen(exHolder->stacks[i].classHolder.generic) + 1);
        xmlAddChild( node, cdataPtr );
      }
      // class source
      if (exHolder->stacks[i].classHolder.source != NULL) {
        node = xmlNewChild(stack, NULL, BAD_CAST "s", NULL);
        cdataPtr = xmlNewCDataBlock( doc, BAD_CAST exHolder->stacks[i].classHolder.source, strlen(exHolder->stacks[i].classHolder.source) + 1);
        xmlAddChild( node, cdataPtr );
      }

      xmlNodePtr vars = xmlNewChild(stack, NULL, BAD_CAST "vars", NULL);

      recursiveProcessingOfVarsAndTheirFields(exHolder, exHolder->stacks[i].var_count, &vars, exHolder->stacks[i].vars, 0);
    }
  }
  //free(exHolder->stacks);
  hashmap_free(exHolder->object_map);
  xmlChar * mem = docToChar(doc);
  xmlFreeDoc(doc);
  return mem;
}
