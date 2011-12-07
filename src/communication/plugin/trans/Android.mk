LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS:= -Wall
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/.. $(LOCAL_PATH)/../.. $(LOCAL_PATH)/../../.. \
	$(LOCAL_PATH)/../../../.. \
	$(call include-path-for, glib) \
	$(call include-path-for, glib)/glib \

LOCAL_SRC_FILES = plugin_trans.c

LOCAL_MODULE:= libantidotecommplugintrans
LOCAL_MODULE_TAGS := debug eng


include $(BUILD_STATIC_LIBRARY)
