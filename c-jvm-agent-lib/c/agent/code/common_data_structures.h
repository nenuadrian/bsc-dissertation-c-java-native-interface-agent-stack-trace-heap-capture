#ifndef UTIL_C_D_S_H 
#define UTIL_C_D_S_H 

#include <jvmti.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "logging.h"
#include "hashmap.h"
#include <string.h>
#include <stdbool.h>

struct GlobalAgentData {
  jvmtiEnv * jvmti;
};

struct ClassHolder {
  char * signature;
  char * generic;
  char * source;
};

struct VarHolder {
  int object_id;
  char * name;
  char * signature;
  char * generic;

  char * value;
  int array_length;
  bool is_static;

  int field_count;
  VarHolder * fields;
};

struct StackHolder { 
  int var_count;
  int line;
  char * method;
  VarHolder * vars;
  ClassHolder classHolder;
};

struct ExceptionHolder {  
  int caught;
  char * caught_in_method;
  ClassHolder * catcher;

  const char * getMessage;
  int stack_count;
  StackHolder * stacks;
  hashmap_map * object_map;
};

struct ThreadHolder {
  int type;
  void *arg;
};

struct App {
  char * class_path;
  char * process_name;
  char * command;
  char * jvm;
  char * app_hash;
  int work_offline;
};

 struct url_data {
   size_t size;
   char* data;
 };


#endif
