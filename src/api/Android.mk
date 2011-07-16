LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS:= -Wall
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/.. $(LOCAL_PATH)/../..

LOCAL_SRC_FILES = \
					text_encoder.c \
					data_encoder.c \
					json_encoder.c \
					xml_encoder.c

LOCAL_MODULE:= libantidoteapi
LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)
