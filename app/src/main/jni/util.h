//
// Created by 钟龙州 on 16/8/6.
//

#ifndef USECURL_UTIL_H
#define USECURL_UTIL_H

#include <jni.h>

jbyteArray char2jbyteArray(JNIEnv *env, char* pat, unsigned int nSize);
jstring char2Jstring(JNIEnv* env, char* pat);

#endif //USECURL_UTIL_H
