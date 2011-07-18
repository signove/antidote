LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS:= -Wall
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/.. $(LOCAL_PATH)/../..

LOCAL_SRC_FILES = \
			       dim.c \
			       dimutil.c \
			       pmstore.c \
			       pmsegment.c \
			       cfg_scanner.c \
			       epi_cfg_scanner.c \
			       mds.c \
			       metric.c \
			       numeric.c \
			       rtsa.c \
			       enumeration.c \
			       peri_cfg_scanner.c \
			       scanner.c 

LOCAL_MODULE:= libantidotedim
LOCAL_MODULE_TAGS := debug eng

include $(BUILD_STATIC_LIBRARY)
