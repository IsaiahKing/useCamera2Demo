LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

TARGET_ARCH_ABI := armeabi-v7a
TARGET_ABI := android-21-armeabi-v7a

LOCAL_MODULE    := sqlite3
LOCAL_C_INCLUDES := ../../Sample/
LOCAL_SRC_FILES := ../../Sample/sqlite3.c

include $(BUILD_SHARED_LIBRARY)
