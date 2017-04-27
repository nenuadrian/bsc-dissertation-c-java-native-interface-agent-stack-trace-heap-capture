#ifndef __EXC_H_H__
#define __EXC_H_H__

#include "common_data_structures.h"

int callbackException(struct ExceptionHolder * exHolder, jvmtiEnv *jvmti_env, JNIEnv* env, jthread thr, jmethodID method, jlocation location, jobject exception, jmethodID catch_method, jlocation catch_location);
#endif