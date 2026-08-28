LOCAL_PATH := $(call my-dir)
BUILD_SHARED := false

include $(CLEAR_VARS)

TMP_LOCAL_PATH := $(LOCAL_PATH)

ifeq ($(or $(findstring true, $(BUILD_SHARED)), $(findstring 1, $(BUILD_SHARED))), true)
	include $(TMP_LOCAL_PATH)/src/library.mk
else
	include $(TMP_LOCAL_PATH)/src/executable.mk
endif