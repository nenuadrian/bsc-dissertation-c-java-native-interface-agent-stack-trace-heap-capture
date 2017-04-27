#include "common_data_structures.h"

int get_line_number(jvmtiEnv *jvmti, jmethodID method, jlocation location) {
  jvmtiError jvmti_err;
  jint table_size;
  jvmtiLineNumberEntry *table;
  int invocation_line_number = -1;
  jvmtiError res = jvmti->GetLineNumberTable(method, &table_size, &table);

  if (res != JVMTI_ERROR_NONE) {
    return 0;
  }

  for (int i = 0; i < table_size; i++) {
    if (table[i].start_location >= location) {
      if (i >= 1)
        invocation_line_number = table[i - 1].line_number;
      else
        invocation_line_number = table[i].line_number;
      break;
    }
  }

  jvmti->Deallocate((unsigned char *)table);

  return invocation_line_number;
}

void processObjectValue(JNIEnv * env, jobject obj, struct VarHolder * varHolder, jclass * classHolder) {
  jstring string;
  jmethodID out_method;
  if(env->IsSameObject(obj, NULL) == JNI_FALSE) {
    *(classHolder) = env->GetObjectClass(obj);
    out_method = env->GetMethodID(*(classHolder), "toString", "()Ljava/lang/String;");
    string = (jstring)env->CallObjectMethod(obj, out_method);
    if (string != NULL) {
      varHolder->value = (char *) calloc(1000, sizeof(char));
      strcpy(varHolder->value, env->GetStringUTFChars(string, 0));
    }
    env->ReleaseStringUTFChars(string, 0);
  }
}

void evaluateFieldValueRecursively(jvmtiEnv * jvmti_env, JNIEnv * env, jclass classHolder, jobject obj, VarHolder * varHolder, int depth, struct ExceptionHolder * exHolder) {
  /* if (depth > 3) {
     error("DEPTH TOO BIG");
     return;
   }*/
     char buffer[30];
     jvmtiError jvmti_err;
     jfieldID * fields;
     jint int_value;
     jlong long_value;
     jfloat float_value;
     jdouble double_value;
     jobject obj_value;
     jclass classHolder2 = NULL;
     jmethodID out_method;
     jstring string;
     jint accessFlags;
     int map_error;

     jvmti_err = jvmti_env->GetClassFields(classHolder, &varHolder->field_count, &fields);
     if (jvmti_err == JVMTI_ERROR_NONE && varHolder->field_count > 0) {
       varHolder->fields = (struct VarHolder *)malloc(varHolder->field_count * sizeof(struct VarHolder));
       for (int fi = 0; fi < varHolder->field_count; fi ++) {
         varHolder->fields[fi].value = NULL;
         varHolder->fields[fi].name = NULL;
         varHolder->fields[fi].signature = NULL;
         varHolder->fields[fi].generic = NULL;
         varHolder->fields[fi].fields = NULL;
         varHolder->fields[fi].object_id = -1;
         varHolder->fields[fi].array_length = -1;
         varHolder->fields[fi].is_static = false;

         jvmti_err = jvmti_env->GetFieldName(classHolder, fields[fi],
           &varHolder->fields[fi].name,
           &varHolder->fields[fi].signature,
           &varHolder->fields[fi].generic);

         if (strcmp("this$0", varHolder->fields[fi].name) == 0) continue; // SKIP

         if (jvmti_err == JVMTI_ERROR_NONE) {
           jvmti_env->GetFieldModifiers(classHolder, fields[fi], &accessFlags);

        // IS STATIC?
        if (accessFlags & 0x0008) {
          varHolder->fields[fi].is_static = true;

          switch (varHolder->fields[fi].signature[0]) {
            // object?
            case 'L':
            obj_value = env->GetStaticObjectField(classHolder, fields[fi]);
            jvmti_env->GetObjectHashCode(obj_value, &varHolder->fields[fi].object_id);
            VarHolder * object_in_map;

            sprintf(buffer, "%d", varHolder->fields[fi].object_id);
            //debug ("[EXCEPTION] Processing field %s %s - depth %d", varHolder->fields[fi].name, buffer, depth);

            char *object_map_key;
            object_map_key = (char*)malloc( sizeof(char) * ( 40 + 1 ) );
            sprintf(object_map_key, "%d", varHolder->fields[fi].object_id);
            map_error = hashmap_get(exHolder->object_map, object_map_key, (void**)(&object_in_map));

            if (map_error == MAP_OK) {
              // object already recorded
              varHolder->fields[fi].name = NULL;
              varHolder->fields[fi].signature = NULL;
              varHolder->fields[fi].generic = NULL;
            } else {
              processObjectValue(env, obj_value, &varHolder->fields[fi], &classHolder2);
              map_error = hashmap_put(exHolder->object_map, object_map_key, (void**)(&varHolder->fields[fi]));

              if (varHolder->fields[fi].value != NULL)
                evaluateFieldValueRecursively(jvmti_env, env, classHolder2, obj_value, &varHolder->fields[fi], depth + 1, exHolder);
            }
            break;
            // array?
            case '[':
             /* jobjectArray arr;
              int length; length = -1;

              jvmti_err =  jvmti_env->GetLocalObject( thr, i, entry.slot, &obj_value);
              if (jvmti_err == JVMTI_ERROR_NONE) {
                jvmti_env->GetObjectHashCode(obj_value, &varHolder.object_id);

                arr = (jobjectArray)obj_value;

                length = env->GetArrayLength(arr);
                if (length < 0) length = -1;
              }
              varHolder.array_length = length;*/
              break;

              case 'Z':
              varHolder->fields[fi].value = (char *) calloc(33, sizeof(char));
              int_value = env->GetStaticIntField(classHolder, fields[fi]);
              sprintf(varHolder->fields[fi].value, "%d", int_value);
              break;

              case 'B':
              //jvmti_err =jvmti_env->GetLocalInt( thr, i, entry.slot, &int_value);
              //sprintf(varHolder.value, "%d", int_value);
              break;

              case 'S':
              //jvmti_err =jvmti_env->GetLocalInt( thr, i, entry.slot, &int_value);
              //sprintf(varHolder.value, "%d", int_value);
              break;

              case 'C':
              //jvmti_err =jvmti_env->GetLocalInt( thr, i, entry.slot, &int_value);
              //sprintf(varHolder.value, "%d", int_value);
              break;

              case 'I':
              varHolder->fields[fi].value = (char *) calloc(33, sizeof(char));
              int_value = env->GetStaticIntField(classHolder, fields[fi]);
              sprintf(varHolder->fields[fi].value, "%d", int_value);
              break;

              case 'J':
              //jvmti_err =jvmti_env->GetLocalLong( thr, i, entry.slot, &long_value);
              //sprintf(varHolder.value, "%ld", long_value);
              break;

              case 'F':
              //jvmti_err =jvmti_env->GetLocalFloat( thr, i, entry.slot, &float_value);
             // sprintf(varHolder.value, "%f", float_value);
              break;

              case 'D':
             // jvmti_err =jvmti_env->GetLocalDouble( thr, i, entry.slot, &double_value);
              //sprintf(varHolder.value, "%lf", double_value);
              break;
              default:
              info("not handled: %s\n", varHolder->fields[fi].signature);
            }

        } else { // end static
          switch (varHolder->fields[fi].signature[0]) {
            // object?
            case 'L':
            obj_value = env->GetObjectField(obj, fields[fi]);
            jvmti_env->GetObjectHashCode(obj_value, &varHolder->fields[fi].object_id);

            VarHolder * object_in_map;

            sprintf(buffer, "%d", varHolder->fields[fi].object_id);
            //debug ("[EXCEPTION] Processing field %s %s - depth %d", varHolder->fields[fi].name, buffer, depth);

            char *object_map_key;
            object_map_key = (char*)malloc( sizeof(char) * ( 40 + 1 ) );
            sprintf(object_map_key, "%d", varHolder->fields[fi].object_id);
            map_error = hashmap_get(exHolder->object_map, object_map_key, (void**)(&object_in_map));

            if (map_error == MAP_OK) {
              // object already recorded
              varHolder->fields[fi].name = NULL;
              varHolder->fields[fi].signature = NULL;
              varHolder->fields[fi].generic = NULL;
            } else {
              processObjectValue(env, obj_value, &varHolder->fields[fi], &classHolder2);
              map_error = hashmap_put(exHolder->object_map, object_map_key, (void**)(&varHolder->fields[fi]));

              if (varHolder->fields[fi].value != NULL)
                evaluateFieldValueRecursively(jvmti_env, env, classHolder2, obj_value, &varHolder->fields[fi], depth + 1, exHolder);
            }

            break;
            // array?
            case '[':
             /* jobjectArray arr;
              int length; length = -1;

              jvmti_err =  jvmti_env->GetLocalObject( thr, i, entry.slot, &obj_value);
              if (jvmti_err == JVMTI_ERROR_NONE) {
                jvmti_env->GetObjectHashCode(obj_value, &varHolder.object_id);

                arr = (jobjectArray)obj_value;

                length = env->GetArrayLength(arr);
                if (length < 0) length = -1;
              }
              varHolder.array_length = length;*/
              break;

              case 'Z':
              varHolder->fields[fi].value = (char *) calloc(33, sizeof(char));
              int_value = env->GetIntField( obj, fields[fi]);
              sprintf(varHolder->fields[fi].value, "%d", int_value);
              break;

              case 'B':
              //jvmti_err =jvmti_env->GetLocalInt( thr, i, entry.slot, &int_value);
              //sprintf(varHolder.value, "%d", int_value);
              break;

              case 'S':
              //jvmti_err =jvmti_env->GetLocalInt( thr, i, entry.slot, &int_value);
              //sprintf(varHolder.value, "%d", int_value);
              break;

              case 'C':
              //jvmti_err =jvmti_env->GetLocalInt( thr, i, entry.slot, &int_value);
              //sprintf(varHolder.value, "%d", int_value);
              break;

              case 'I':
              varHolder->fields[fi].value = (char *) calloc(33, sizeof(char));
              int_value = env->GetIntField(obj, fields[fi]);
              sprintf(varHolder->fields[fi].value, "%d", int_value);
              break;

              case 'J':
              //jvmti_err =jvmti_env->GetLocalLong( thr, i, entry.slot, &long_value);
              //sprintf(varHolder.value, "%ld", long_value);
              break;

              case 'F':
              //jvmti_err =jvmti_env->GetLocalFloat( thr, i, entry.slot, &float_value);
             // sprintf(varHolder.value, "%f", float_value);
              break;

              case 'D':
             // jvmti_err =jvmti_env->GetLocalDouble( thr, i, entry.slot, &double_value);
              //sprintf(varHolder.value, "%lf", double_value);
              break;
              default:
              info("not handled: %s\n", varHolder->fields[fi].signature);
            }
          }
      } // not static
    }
  }
}




void processVar(jvmtiEnv * jvmti_env, JNIEnv * env, jthread thr, int i, jvmtiLocalVariableEntry * entry, struct VarHolder * varHolder, struct ExceptionHolder * exHolder) {
  jvmtiError jvmti_err;
  char buffer[30];
  jint int_value;
  jlong long_value;
  jfloat float_value;
  jdouble double_value;
  jobject obj_value = NULL;
  jclass classHolder;
  VarHolder * object_in_map;
  char *object_map_key;
  int map_error;

  varHolder->name = NULL;
  varHolder->signature = NULL;
  varHolder->generic = NULL;
  varHolder->fields = NULL;
  varHolder->object_id = -1;
  varHolder->array_length = -1;

  if (entry->name != NULL && entry->signature != NULL) {
    varHolder->name = entry->name;
    varHolder->signature = entry->signature;
    varHolder->generic = entry->generic_signature;
    varHolder->value = NULL;

    switch (entry->signature[0]) {
      case 'L':
      jvmti_err = jvmti_env->GetLocalObject(thr, i, entry->slot, &obj_value);
      if (jvmti_err != JVMTI_ERROR_NONE) {
       /* jvmti_err = jvmti_env->GetLocalInstance(thr, i, &obj_value); */
          varHolder->name = NULL;
          return ;
      }

      jvmti_env->GetObjectHashCode(obj_value, &varHolder->object_id);

      object_map_key = (char*)calloc(33, sizeof(char));
      sprintf(object_map_key, "%d", varHolder->object_id);

      map_error = hashmap_get(exHolder->object_map, object_map_key, (void**)(&object_in_map));
      if (map_error == MAP_OK) {
        varHolder->name = NULL;
        varHolder->signature = NULL;
        varHolder->generic = NULL;
      } else {
        processObjectValue(env, obj_value, varHolder, &classHolder);
        map_error = hashmap_put(exHolder->object_map, object_map_key, (void**)(varHolder));

        if (varHolder->value != NULL)
          evaluateFieldValueRecursively(jvmti_env, env, classHolder, obj_value, varHolder, 0, exHolder);
      }
      break;
      case '[':
        jobjectArray arr;

        jvmti_err =  jvmti_env->GetLocalObject( thr, i, entry->slot, &obj_value);
        jvmti_env->GetObjectHashCode(obj_value, &varHolder->object_id);

        object_map_key = (char*)calloc(33, sizeof(char));
        sprintf(object_map_key, "%d", varHolder->object_id);

        map_error = hashmap_get(exHolder->object_map, object_map_key, (void**)(&object_in_map));
        if (map_error == MAP_OK) {
          // object already recorded
          varHolder->name = NULL;
          varHolder->signature = NULL;
          varHolder->generic = NULL;
        } else {
          processObjectValue(env, obj_value, varHolder, &classHolder);
          map_error = hashmap_put(exHolder->object_map, object_map_key, (void**)(varHolder));

          arr = (jobjectArray)obj_value;
          varHolder->array_length = env->GetArrayLength(arr);

          //if (varHolder->value != NULL)
          //  evaluateFieldValueRecursively(jvmti_env, env, classHolder, obj_value, varHolder, 0, exHolder);
        }
        break;

        case 'Z':
        jvmti_err =jvmti_env->GetLocalInt( thr, i, entry->slot, &int_value);
        if (jvmti_err != JVMTI_ERROR_NONE) { varHolder->name = NULL; return; }
        varHolder->value = (char *) calloc(20, sizeof(char));
        sprintf(varHolder->value, "%d", int_value);
        break;

        case 'B':
        jvmti_err =jvmti_env->GetLocalInt( thr, i, entry->slot, &int_value);
        if (jvmti_err != JVMTI_ERROR_NONE) { varHolder->name = NULL; return; }
        varHolder->value = (char *) calloc(10, sizeof(char));
        sprintf(varHolder->value, "%d", int_value);
        break;

        case 'S':
        jvmti_err =jvmti_env->GetLocalInt( thr, i, entry->slot, &int_value);
        if (jvmti_err != JVMTI_ERROR_NONE) { varHolder->name = NULL; return; }
        varHolder->value = (char *) calloc(18, sizeof(char));
        sprintf(varHolder->value, "%d", int_value);
        break;

        case 'C':
        jvmti_err =jvmti_env->GetLocalInt( thr, i, entry->slot, &int_value);
        if (jvmti_err != JVMTI_ERROR_NONE) { varHolder->name = NULL; return; }
        varHolder->value = (char *) calloc(100, sizeof(char));
        sprintf(varHolder->value, "%c", int_value);
        break;

        case 'I':
        jvmti_err =jvmti_env->GetLocalInt( thr, i, entry->slot, &int_value);
        if (jvmti_err != JVMTI_ERROR_NONE) { varHolder->name = NULL; return; }
        varHolder->value = (char *) calloc(33, sizeof(char));
        sprintf(varHolder->value, "%d", int_value);
        break;

        case 'J':
        jvmti_err =jvmti_env->GetLocalLong( thr, i, entry->slot, &long_value);
        if (jvmti_err != JVMTI_ERROR_NONE) { varHolder->name = NULL; return; }
        varHolder->value = (char *) calloc(70, sizeof(char));
        sprintf(varHolder->value, "%ld", long_value);
        break;

        case 'F':
        jvmti_err =jvmti_env->GetLocalFloat( thr, i, entry->slot, &float_value);
        if (jvmti_err != JVMTI_ERROR_NONE) { varHolder->name = NULL; return; }
        varHolder->value = (char *) calloc(70, sizeof(char));
        sprintf(varHolder->value, "%f", float_value);
        break;

        case 'D':
        jvmti_err =jvmti_env->GetLocalDouble( thr, i, entry->slot, &double_value);
        if (jvmti_err != JVMTI_ERROR_NONE) { varHolder->name = NULL; return; }
        varHolder->value = (char *) calloc(130, sizeof(char));
        sprintf(varHolder->value, "%lf", double_value);
        break;
        default:
        info("not handled: %s\n", entry->signature);
      }

      debug("%s %s", entry->name, varHolder->value);
    }
  }
//http://journals.ecs.soton.ac.uk/java/tutorial/native1.1/implementing/method.html
//https://svn.apache.org/repos/asf/harmony/enhanced/java/trunk/drlvm/vm/vmcore/src/jvmti/jvmti_thread_group.cpp



  static void attachInfoTraceToStack(JNIEnv* env, jobject exception) {
    jclass classHolder = env->FindClass( "java/lang/Throwable");
    if (classHolder == NULL) return;
    jmethodID out_method = env->GetMethodID( classHolder, "getStackTrace", "()[Ljava/lang/StackTraceElement;");
      if (out_method == NULL) return;
      jobjectArray arr = (jobjectArray)env->CallObjectMethod(exception, out_method);
      if (arr == NULL) return;

      jclass classHolder2 = env->FindClass( "java/lang/StackTraceElement");
      if (classHolder2 == NULL) return;
      jmethodID traceConstructor = env->GetMethodID(classHolder2, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
      if (traceConstructor == NULL) return;

      jstring traceClassName = env->NewStringUTF("http://prod.codevolution");
      jstring traceMethodName = env->NewStringUTF("com/5234 ");
      jstring traceSourceName = env->NewStringUTF("ProdAgent");

      jint traceLine = 2;
      jobject newStackElem = env->NewObject(classHolder2, traceConstructor, traceClassName, traceMethodName, traceSourceName, traceLine);

      jsize len = env->GetArrayLength(arr);

      jobjectArray args = env->NewObjectArray(len + 1, env->FindClass( "java/lang/StackTraceElement"), 0);

      for (int j = 0; j < len ; j++){
        env->SetObjectArrayElement(args, j, env->GetObjectArrayElement(arr, j));
      }

      env->SetObjectArrayElement(args, len, newStackElem);
      out_method = env->GetMethodID( classHolder, "setStackTrace", "([Ljava/lang/StackTraceElement;)V");
        env->CallObjectMethod( exception, out_method, args);
      }


      void jmethodToClassHolder(jvmtiEnv *jvmti_env, jmethodID * method, ClassHolder * classHolder) {
        jvmtiError jvmti_err;
        classHolder->signature = NULL;
        classHolder->source = NULL;
        classHolder->generic = NULL;

        jclass declaring_class;
        jvmti_err =jvmti_env->GetMethodDeclaringClass(*(method), &declaring_class);
        if (jvmti_err == JVMTI_ERROR_NONE) {
          jvmti_err =jvmti_env->GetClassSignature( declaring_class, &classHolder->signature, &classHolder->generic);
          if (jvmti_err != JVMTI_ERROR_NONE) {
            classHolder->signature = NULL;
          }
          jvmti_err =jvmti_env->GetSourceFileName(declaring_class, &classHolder->source);
          if (jvmti_err != JVMTI_ERROR_NONE)
            classHolder->source = NULL;
        }
      }

      int callbackException(struct ExceptionHolder * exHolder, jvmtiEnv *jvmti_env, JNIEnv* env, jthread thr, jmethodID method, jlocation location, jobject exception, jmethodID catch_method, jlocation catch_location) {
        jvmtiError jvmti_err;
        jvmtiFrameInfo * frames = ( jvmtiFrameInfo *)calloc(10000, sizeof( jvmtiFrameInfo));
        exHolder->catcher = NULL;
        exHolder->stacks = NULL;

        if (catch_method != NULL) {
          exHolder->catcher = (struct ClassHolder *)malloc(sizeof(struct ClassHolder));
          jmethodToClassHolder(jvmti_env, &catch_method, exHolder->catcher);
          // SKIP jvm caught exception
          if (!strcmp(exHolder->catcher->source, "ClassLoader.java") ||
              !strcmp(exHolder->catcher->source, "URLClassPath.java")) {
            debug("Skipped native exception");
            free(exHolder->catcher);
            return 1;
          }

          exHolder->caught = get_line_number(jvmti_env, catch_method, catch_location);
          jvmti_err = jvmti_env->GetMethodName(catch_method, &exHolder->caught_in_method, NULL, NULL);
          if (jvmti_err != JVMTI_ERROR_NONE) {
            exHolder->caught_in_method = NULL;
          }

          debug ("CAUGHT %s %s", exHolder->catcher->signature,  exHolder->catcher->source);
        } else exHolder->caught = 0;

        exHolder->object_map = hashmap_new();

        jvmti_err = jvmti_env->GetStackTrace(thr, 0, 10000, frames, &exHolder->stack_count);
        if (exHolder->stack_count < 0)   exHolder->stack_count = -1;

        if (jvmti_err == JVMTI_ERROR_NONE && exHolder->stack_count > 0) {

          if (exHolder->stack_count > 100) {
            error("HUUUUGEEE STACK TRACEEEEE");
            error("HUUUUGEEE STACK TRACEEEEE");
            error("HUUUUGEEE STACK TRACEEEEE");
            exHolder->stack_count = 100;
          }


    // TODO deallocate?
          exHolder->stacks = (struct StackHolder *)calloc(exHolder->stack_count, sizeof(struct StackHolder));
          for (int i = 0; i < exHolder->stack_count; i++) {
            exHolder->stacks[i].vars = NULL;
            exHolder->stacks[i].var_count = -1;

            // Variables
            jvmtiLocalVariableEntry * lv_table_ptr;
            int var_count;
            jvmti_err = jvmti_env->GetLocalVariableTable(frames[i].method, &var_count, &lv_table_ptr);

            if (jvmti_err != JVMTI_ERROR_NONE) {
              error("ERROR FETCHING LOCAL VARIABLES %d", jvmti_err);
              if (jvmti_err == 101 || jvmti_err == 104)
                return 1; else continue;
            }

            jmethodToClassHolder(jvmti_env, &frames[i].method, &exHolder->stacks[i].classHolder);
            exHolder->stacks[i].line = get_line_number(jvmti_env, frames[i].method, frames[i].location);

            jvmti_err = jvmti_env->GetMethodName(frames[i].method, &exHolder->stacks[i].method, NULL, NULL);
            if (jvmti_err != JVMTI_ERROR_NONE) {
              exHolder->stacks[i].method = NULL;
            }

            exHolder->stacks[i].var_count = var_count;

            if (exHolder->stacks[i].var_count > 100) {
              exHolder->stacks[i].var_count = 100;
              error("WAAAY TOOO MANYY VARIABLEEES");
            }

            debug("[EXCEPTION] Process %d vars", exHolder->stacks[i].var_count);
            if (exHolder->stacks[i].var_count > 0) {
              // TODO deallocate?
              exHolder->stacks[i].vars = (struct VarHolder *)calloc(exHolder->stacks[i].var_count, sizeof(struct VarHolder));
              for (int j = 0; j < var_count; j++)
                processVar(jvmti_env, env, thr, i, &lv_table_ptr[j], &exHolder->stacks[i].vars[j], exHolder);
            }
            jvmti_env->Deallocate ((unsigned char *) lv_table_ptr);
          }
        }

        attachInfoTraceToStack(env, exception);

        // getMessage
        jclass classHolder = env->FindClass( "java/lang/Throwable");
        jmethodID out_method = env->GetMethodID( classHolder, "getMessage", "()Ljava/lang/String;");
        jstring exString = (jstring)env->CallObjectMethod(exception, out_method);
        if (exString != NULL) {
          exHolder->getMessage = env->GetStringUTFChars(exString, 0);
        } else exHolder->getMessage = NULL;

        env->ReleaseStringUTFChars(exString, 0);

        free(frames);
        return 0;
      }
