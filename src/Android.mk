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

# shared lib

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(common_SRC_FILES)
LOCAL_CFLAGS := $(common_CFLAGS)
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/..

LOCAL_MODULE:= libantidote
LOCAL_MODULE_TAGS := debug eng
LOCAL_PRELINK_MODULE:= false

LOCAL_WHOLE_STATIC_LIBRARIES := libantidoteapi libantidotecomm libantidotecommplugin \
		        libantidotecommparser \
			libantidotedim libantidoteutil libantidotespecializations

include $(BUILD_SHARED_LIBRARY)

# sample agent

include $(CLEAR_VARS)

LOCAL_SRC_FILES := sample_agent.c
LOCAL_CFLAGS := $(common_CFLAGS)
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/..

LOCAL_STATIC_LIBRARIES := libantidotecommpluginimpl libantidote

LOCAL_MODULE:= ieee_agent
LOCAL_MODULE_TAGS := debug eng

include $(BUILD_EXECUTABLE)

# sample manager

include $(CLEAR_VARS)

LOCAL_SRC_FILES := sample_manager.c
LOCAL_CFLAGS := $(common_CFLAGS)
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/..

LOCAL_STATIC_LIBRARIES := libantidotecommpluginimpl libantidote

LOCAL_MODULE:= ieee_manager
LOCAL_MODULE_TAGS := debug eng

include $(BUILD_EXECUTABLE)

