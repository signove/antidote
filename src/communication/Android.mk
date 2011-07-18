LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS:= -Wall
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/.. $(LOCAL_PATH)/../..

LOCAL_SRC_FILES = \
		   association.c \
                   communication.c \
                   configuring.c \
                   disassociating.c \
		   agent_ops.c \
                   extconfigurations.c \
                   fsm.c \
                   service.c \
                   operating.c \
                   stdconfigurations.c \
                   context_manager.c

LOCAL_MODULE:= libantidotecomm
LOCAL_MODULE_TAGS := debug eng

include $(BUILD_STATIC_LIBRARY)
