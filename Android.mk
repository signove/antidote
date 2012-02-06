BASE_PATH:= $(call my-dir)

include $(BASE_PATH)/src/communication/plugin/Android.mk
include $(BASE_PATH)/src/communication/plugin/trans/Android.mk
include $(BASE_PATH)/src/communication/plugin/android/Android.mk
include $(BASE_PATH)/src/communication/parser/Android.mk
include $(BASE_PATH)/src/communication/Android.mk
include $(BASE_PATH)/src/specializations/Android.mk
include $(BASE_PATH)/src/util/Android.mk
include $(BASE_PATH)/src/dim/Android.mk
include $(BASE_PATH)/src/api/Android.mk
include $(BASE_PATH)/src/trans/Android.mk
include $(BASE_PATH)/src/Android.mk
