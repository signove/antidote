LOCAL_PATH:= $(call my-dir)

LOCAL_CFLAGS:= -Wall

common_SRC_FILES := \
	manager.c agent.c

common_CFLAGS := -Wall

# static lib

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(common_SRC_FILES)
LOCAL_CFLAGS := $(common_CFLAGS)
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/..

LOCAL_MODULE:= libantidote
LOCAL_MODULE_TAGS := debug eng

LOCAL_WHOLE_STATIC_LIBRARIES := libantidoteapi libantidotecomm libantidotecommplugin \
				libantidotecommparser \
				libantidotedim libantidoteutil libantidotespecializations

include $(BUILD_STATIC_LIBRARY)

# Bluetooth HDP manager and healthd service

include $(CLEAR_VARS)

LOCAL_SRC_FILES := healthd_android.c
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/..

LOCAL_STATIC_LIBRARIES := libantidotecommpluginandroid libantidote

LOCAL_MODULE:= healthd
LOCAL_MODULE_TAGS := debug eng

include $(BUILD_SHARED_LIBRARY)
