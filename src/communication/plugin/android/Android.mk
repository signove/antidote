LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS:= -Wall
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/.. $(LOCAL_PATH)/../.. $(LOCAL_PATH)/../../.. \
	$(LOCAL_PATH)/../../../..

LOCAL_SRC_FILES = plugin_android.c

LOCAL_MODULE:= libantidotecommpluginandroid
LOCAL_MODULE_TAGS := debug eng


include $(BUILD_STATIC_LIBRARY)
