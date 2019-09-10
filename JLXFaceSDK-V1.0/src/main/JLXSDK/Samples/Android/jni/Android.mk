LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

TARGET_ARCH_ABI := armeabi-v7a
TARGET_ABI := android-21-armeabi-v7a

#TARGET_ARCH_ABI := arm64-v8a
#TARGET_ABI := android-21-arm64-v8a

#TARGET_ARCH_ABI := armeabi
#TARGET_ABI := android-21-armeabi


LOCAL_MODULE := ftdi-prebuilt
LOCAL_SRC_FILES := ../../../Lib/Android/armeabi/libftd3xx.so

# LOCAL_MODULE := sqlite3
# LOCAL_SRC_FILES := ../../sqlite3so/libs/armeabi-v7a/libsqlite3.so

include $(PREBUILT_SHARED_LIBRARY)

LOCAL_C_INCLUDES :=  ../../Samples/Sample/ ../../../Include/ ../../../Include/Android/jni/include/

LOCAL_CFLAGS := -std=c++11 -DLINUX -DANDROID -DFACE_RECOGNITION -L../../../Lib/Android/armeabi -O3
#-mfloat-abi=hardfp -DNDEBUG -mfpu=vfp -marm

include ../../../Include/Android/jni/OpenCV.mk

LOCAL_MODULE    := gti_classify
LOCAL_SRC_FILES := 	../../Sample/Classify.cpp \
					../../Sample/JLXsqlite3.cpp \
					jni_classify.cpp

LOCAL_LDLIBS += -L../../../Lib/Android/armeabi -L../../sqlite3so/libs/armeabi-v7a -llog -ldl -lstdc++ -lc -lz -lm -landroid -latomic -lgti_android -lstdc++ -lsqlite3

include $(BUILD_SHARED_LIBRARY)
