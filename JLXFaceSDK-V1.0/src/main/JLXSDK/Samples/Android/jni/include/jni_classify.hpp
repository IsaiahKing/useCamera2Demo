#ifndef __JNI_CLASSIFY_HPP
#define __JNI_CLASSIFY_HPP

#include <jni.h>
#include <string>
#include <stdio.h>
#include <exception>
#include "jni_detect.hpp"
#include "jni_jlx_util.hpp"

#define  JNI_LOG_TAG    "JLX_JNI"
#define  JLX_JNI_INIT_OK 0
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,JNI_LOG_TAG,__VA_ARGS__)
#define  ELOG(...)  __android_log_print(ANDROID_LOG_ERROR,JNI_LOG_TAG,__VA_ARGS__)

#define  JCLASS_PATH "com/jenkolux/facedetect/data/JlxBox"
#define  FACE_ERROR 0
#define  FACE_CHECK 1


typedef enum {
    RegisterOK = 0,
    RegisterFail = 1
};

#endif