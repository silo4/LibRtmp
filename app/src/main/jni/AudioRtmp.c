//
// Created by 钟龙州 on 2016/12/8.
//

#include <stdint.h>
#include <malloc.h>
#include <memory.h>
#include "logger.h"
#include "audio_rtmp_LibRtmp.h"
#include "audio_rtmp_context.h"

static const char* TAG = "native_rtmp++++++";

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    LOGI(TAG, "JNI OnLoad !!!!!!!!!!!!!!");
    JNIEnv *env;

    if ((*vm)->GetEnv(vm, (void **)&env, CURL_JNI_VERSION) != JNI_OK) {
        return -1;
    }

    (*env)->GetJavaVM(env, &sjvm);

    return CURL_JNI_VERSION;
}

JNIEXPORT jint JNICALL Java_audio_rtmp_LibRtmp_initRtmp
        (JNIEnv * env, jobject thiz, jstring jurl)
{
    LOGI(TAG, "init rtmp");
    char* pUrl = NULL;
    if (jurl == NULL)
    {
        LOGE(TAG, "url is empty")
        return JNI_FALSE;
    }

    pUrl = (char *)(*env)->GetStringUTFChars(env, jurl, NULL);
    if (pUrl == NULL)
    {
        LOGE(TAG, "get url failed")
        return JNI_FALSE;
    }

    LOGI(TAG, "rtmp url [%s]", pUrl);

    if (pRtmpContext == NULL)
    {
        pRtmpContext = (StRtmpContext*)malloc(sizeof(StRtmpContext));
    }

    pRtmpContext->pRtmp = RTMP_Alloc();
    RTMP_Init(pRtmpContext->pRtmp);

    if (RTMP_SetupURL(pRtmpContext->pRtmp, pUrl) == FALSE)
    {
        LOGE(TAG, "rtmp set url failed");
        RTMP_Free(pRtmpContext->pRtmp);
        pRtmpContext->pRtmp = NULL;
        return JNI_FALSE;
    }

    RTMP_EnableWrite(pRtmpContext->pRtmp);

    //connect server
    if (RTMP_Connect(pRtmpContext->pRtmp, NULL) == FALSE)
    {
        LOGE(TAG, "rtmp connect server failed");
        RTMP_Free(pRtmpContext->pRtmp);
        pRtmpContext->pRtmp = NULL;
        return JNI_FALSE;
    }

    if (RTMP_ConnectStream(pRtmpContext->pRtmp, 0) == FALSE)
    {
        LOGE(TAG, "rtmp connect stream failed");
        RTMP_Close(pRtmpContext->pRtmp);
        RTMP_Free(pRtmpContext->pRtmp);
        pRtmpContext->pRtmp = NULL;
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

JNIEXPORT jint JNICALL Java_audio_rtmp_LibRtmp_sendAudio
        (JNIEnv *env, jobject thiz, jbyteArray jaudiobuffer, jint jbuffersize, jint jiskey, jint jts)
{
    LOGI(TAG, "send rtmp audio size[%d] ", jbuffersize);

    if (jbuffersize <= 0)
    {
        LOGE(TAG, "send rtmp failed for buffer size is 0");
        return JNI_FALSE;
    }
    LOGI(TAG, "rtmp 1");
    jbyte* pBuffer = (*env)->GetByteArrayElements(env, jaudiobuffer, 0);
//    unsigned char* buf = (unsigned char*)pBuffer;
    int len = jbuffersize;

    if (pBuffer == NULL)
    {
        LOGE(TAG, "get buffer failed");
        return JNI_FALSE;
    }
    LOGI(TAG, "rtmp 2");
//    unsigned int timeoffset;
//    timeoffset = RTMP_GetTime() - start_time;
//
//    buf += 7;
//    len += 7;
//    int ret = -1;
//    if (len > 0) {
//        LOGI(TAG, "rtmp 3");
//        RTMPPacket * packet;
//        unsigned char * body;
//
//        packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+len+2);
//        memset(packet,0,RTMP_HEAD_SIZE);
//        LOGI(TAG, "rtmp 4");
//        packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
//        body = (unsigned char *)packet->m_body;
//
//        /*AF 01 + AAC RAW data*/
//        body[0] = 0xAF;
//        body[1] = 0x01;
//        memcpy(&body[2],buf,len);
//        LOGI(TAG, "rtmp 5");
//        packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
//        packet->m_nBodySize = len+2;
//        packet->m_nChannel = 0x04;
//        packet->m_nTimeStamp = timeoffset;
//        packet->m_hasAbsTimestamp = 0;
//        packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
//        packet->m_nInfoField2 = pRtmpContext->pRtmp->m_stream_id;
//
//        /*调用发送接口*/
//        ret = RTMP_SendPacket(pRtmpContext->pRtmp,packet,TRUE);
//        LOGI(TAG, "rtmp 6");
////        free(packet);
//        LOGI(TAG, "rtmp 7");
//        (*env)->ReleaseByteArrayElements(env, jaudiobuffer, pBuffer, JNI_FALSE);
//        LOGI(TAG, "rtmp 8");
//        (*env)->DeleteLocalRef(env, jaudiobuffer);
//        LOGI(TAG, "rtmp 9");
//    }
//    LOGI(TAG, "rtmp 10");

//    return ret;

    int size = jbuffersize + 2;
    RTMPPacket packet;
    RTMPPacket_Reset(&packet);
    RTMPPacket_Alloc(&packet, size);
    LOGI(TAG, "rtmp 3");
    int i = 0;
    packet.m_body[i++] = 0xAF;
    packet.m_body[i++] = 0x01;
    memcpy(&packet.m_body[i], pBuffer, len);
    LOGI(TAG, "rtmp 4");
//    unsigned int timeoffset;
//    timeoffset = RTMP_GetTime() - start_time;

    packet.m_headerType  = RTMP_PACKET_SIZE_MEDIUM;
    packet.m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet.m_hasAbsTimestamp = 0;
    packet.m_nChannel   = 0x04;
    packet.m_nTimeStamp = (unsigned int)jts;
    packet.m_nInfoField2 = pRtmpContext->pRtmp->m_stream_id;
    packet.m_nBodySize  = size;
    LOGI(TAG, "rtmp 5, ts[%d]", packet.m_nTimeStamp);
    //调用发送接口
    int nRet = RTMP_SendPacket(pRtmpContext->pRtmp, &packet, TRUE);
    LOGI(TAG, "rtmp 6");
    RTMPPacket_Free(&packet);//释放内存
    LOGI(TAG, "rtmp 7");
    (*env)->ReleaseByteArrayElements(env, jaudiobuffer, pBuffer, JNI_FALSE);
    (*env)->DeleteLocalRef(env, jaudiobuffer);
    LOGI(TAG, "rtmp 8");

    return nRet;
}

JNIEXPORT jint JNICALL Java_audio_rtmp_LibRtmp_sendAudioSpec
        (JNIEnv * env, jobject thiz, jbyteArray jbuffer, jint jbuffersize)
{
    LOGI(TAG, "send rtmp spec buffer size[%d] ", jbuffersize);
//    if (jbuffersize <= 0)
//    {
//        LOGE(TAG, "send rtmp spec failed for buffer size is 0");
//        return JNI_FALSE;
//    }
//
//    jbyte* pBuffer = (*env)->GetByteArrayElements(env, jbuffer, 0);
//
//    if (pBuffer == NULL)
//    {
//        LOGE(TAG, "get spec buffer failed");
//        return JNI_FALSE;
//    }
//
//    RTMPPacket* packet;
//    unsigned char * body;
//    int len;
//
//    len = jbuffersize;
//
//    packet = (RTMPPacket*)malloc(RTMP_HEAD_SIZE + 2);
//
//    memset(packet, 0, RTMP_HEAD_SIZE);
//    LOGI(TAG, "rtmp spec 1");
//    packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
//    body = (unsigned char*)packet->m_body;
//
//    /*AF 00 + AAC RAW data*/
//    body[0] = 0xAF;
//    body[1] = 0x00;
//    memcpy(&body[2], (unsigned char * )pBuffer, len);
//
//    LOGI(TAG, "rtmp spec 2");
//    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
//    packet->m_nBodySize = (unsigned int)len+2;
//    packet->m_nChannel = 0x04;
//    packet->m_nTimeStamp = 0;
//    packet->m_hasAbsTimestamp = 0;
//    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
//    packet->m_nInfoField2 = pRtmpContext->pRtmp->m_stream_id;
//    LOGI(TAG, "rtmp spec 3");
//    /*调用发送接口*/
//    int ret = RTMP_SendPacket(pRtmpContext->pRtmp, packet, TRUE);
////    free(packet);
//
//    LOGI(TAG, "rtmp spec 4");
//    (*env)->ReleaseByteArrayElements(env, jbuffer, pBuffer, JNI_FALSE);
//    LOGI(TAG, "rtmp spec 5");
//    (*env)->DeleteLocalRef(env, jbuffer);
//    LOGI(TAG, "rtmp spec 6");
//
//    start_time = RTMP_GetTime();
//    return ret;


    RTMPPacket packet;
    RTMPPacket_Reset(&packet);
    RTMPPacket_Alloc(&packet, 4);

    packet.m_body[0] = 0xAF;  // AAC format 44kHZ
    packet.m_body[1] = 0x00;
//    packet.m_body[2] = 0x11;
//    packet.m_body[3] = 0x90;//0x10修改为0x90,2016-1-19

    packet.m_body[2] = 0x14;
    packet.m_body[3] = 0x10;

    packet.m_headerType  = RTMP_PACKET_SIZE_MEDIUM;
    packet.m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet.m_hasAbsTimestamp = 0;
    packet.m_nChannel   = 0x04;
    packet.m_nTimeStamp = 0;
    packet.m_nInfoField2 = pRtmpContext->pRtmp;
    packet.m_nBodySize  = 4;

    //调用发送接口
    int nRet = RTMP_SendPacket(pRtmpContext->pRtmp, &packet, TRUE);
    RTMPPacket_Free(&packet);//释放内存

    return nRet;

}

JNIEXPORT jint JNICALL Java_audio_rtmp_LibRtmp_uninitRtmp
        (JNIEnv *env, jobject thiz)
{
    LOGI(TAG, "uninit rtmp");
    if (pRtmpContext == NULL)
    {
        LOGE(TAG, "uninit failed for rtmp context is null");
        return JNI_FALSE;
    }

    if (pRtmpContext->pRtmp == NULL)
    {
        LOGE(TAG, "uninit failed for rtmp is null");
        return JNI_FALSE;
    }

    RTMP_Close(pRtmpContext->pRtmp);
    RTMP_Free(pRtmpContext->pRtmp);

    pRtmpContext->pRtmp = NULL;

    free(pRtmpContext);
    pRtmpContext = NULL;

    return JNI_TRUE;
}

JNIEXPORT void JNICALL Java_audio_rtmp_LibRtmp_setDebug
        (JNIEnv *env, jobject thiz, jboolean isDebug)
{
    SETDEBUG(isDebug);
}