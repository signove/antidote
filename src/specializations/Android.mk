LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS:= -Wall
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/.. $(LOCAL_PATH)/../..

LOCAL_SRC_FILES = blood_pressure_monitor.c \
		   pulse_oximeter.c \
		   weighing_scale.c \
		   glucometer.c

LOCAL_MODULE:= libantidotespecializations
LOCAL_MODULE_TAGS := debug eng

include $(BUILD_STATIC_LIBRARY)
