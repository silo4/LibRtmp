##################################

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vrtmp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/.
LOCAL_C_INCLUDES += $(NDK_PATH)/platforms/$(TARGET_PLATFORM)/arch-$(TARGET_ARCH)/usr/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/librtmp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

#jni
LOCAL_SRC_FILES += AudioRtmp.c \
                   util.c \
                   javamethod.c

#librtmp
LOCAL_SRC_FILES += librtmp/amf.c \
                   librtmp/hashswf.c \
                   librtmp/log.c \
                   librtmp/parseurl.c \
                   librtmp/rtmp.c


#支持for循环什么的需要这个标准
LOCAL_CFLAGS += -std=c99

#编译优化
LOCAL_CFLAGS += -O3
LOCAL_LDFLAGS := -O3
LOCAL_ARM_NEON := true

LOCAL_LDLIBS := -llog -lz
LOCAL_LDLIBS += $(LOCAL_PATH)/linkso/libcccccc.so
LOCAL_LDLIBS += $(LOCAL_PATH)/linkso/libsss.so

include $(BUILD_SHARED_LIBRARY)

