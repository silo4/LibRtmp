//
// Created by 钟龙州 on 2016/12/2.
//

#ifndef TESTFAAC_AUDIO_RTMP_CONTEXT_H
#define TESTFAAC_AUDIO_RTMP_CONTEXT_H

#include "javamethod.h"
#include "librtmp/rtmp.h"

#define CURL_JNI_VERSION JNI_VERSION_1_6
#define RTMP_HEAD_SIZE (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)

static JavaVM *sjvm;

static inline JNIEnv *get_env() {
    JNIEnv *env;
    (*sjvm)->GetEnv(sjvm, (void **)&env, CURL_JNI_VERSION);
    return env;
}

typedef struct StRtmpContext
{
    RTMP* pRtmp;

}StRtmpContext;

StRtmpContext* pRtmpContext;
unsigned int start_time;

#endif //TESTFAAC_AUDIO_RTMP_CONTEXT_H
