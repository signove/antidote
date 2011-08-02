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

# Bluetooth HDP manager and healthd service

include $(CLEAR_VARS)

LOCAL_SRC_FILES := healthd_service.c
LOCAL_CFLAGS := $(common_CFLAGS)
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/.. \
	$(call include-path-for, glib) \
	$(call include-path-for, glib)/glib \
	$(call include-path-for, gio) \
	$(call include-path-for, dbus) \
	$(call include-path-for, dbus-glib) \
	$(call include-path-for, dbus-glib)/dbus

LOCAL_STATIC_LIBRARIES := libantidotecommpluginbluez libantidote \
			libdbus-glib_static \
			libgobject_static libgio_static \
			libgthread_static \
			libglib_static

LOCAL_SHARED_LIBRARIES := libdbus

LOCAL_MODULE:= healthd
LOCAL_MODULE_TAGS := debug eng

# FIXME .conf file
#dbusstuffdir = $(libdir)/../share/dbus-1/system-services/
#dbusstuff_DATA = src/resources/com.signove.health.service

include $(BUILD_EXECUTABLE)
