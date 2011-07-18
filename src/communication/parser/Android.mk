LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS:= -Wall
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/.. $(LOCAL_PATH)/../.. $(LOCAL_PATH)/../../..

LOCAL_SRC_FILES = \
		      decoder_ASN1.c \
                      struct_cleaner.c \
                      encoder_ASN1.c

LOCAL_MODULE:= libantidotecommparser
LOCAL_MODULE_TAGS := debug eng

include $(BUILD_STATIC_LIBRARY)
