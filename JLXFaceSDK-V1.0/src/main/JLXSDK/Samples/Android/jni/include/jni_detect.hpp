#ifndef __JNI_DETECT_HPP
#define __JNI_DETECT_HPP

#include <jni.h>
#include <string>
#include <stdio.h>
#include <exception>
#include "Samples.hpp"
#include "jni_jlx_util.hpp"
#include <facedetect/src/main/cpp/mtcnn.h>

#define  FACE_NORMAL 0
#define  FACE_MAX 1
#define  FACE_ALIGN 2

typedef enum {
    DETECT_OK = 0,
    DETECT_FAIL = 1
}MTCNNStatus;

typedef enum {
    DETECT_MODEL_FILE_NO_EXIST = 20001,
    DETECT_NO_POINT = 20002,
    DETECT_FILE_ACCESS_FAIL = 20003,
    DETECT_STATUS_UNAVAILABLE = 20004
}DetectErrorType;

class JNIDetect{
public:
    JNIDetect(){};
    ~JNIDetect(){};

static JNIEXPORT jint JNICALL
    InitDetectModel(JNIEnv* env,jobject obj,
                    jstring modelPath);

static JNIEXPORT jobject  JNICALL
    detectFace(JNIEnv* env,jobject obj,
               jbyteArray imgData,
               jint width,jint height,
               jint faceMode);

static JNIEXPORT jboolean JNICALL
    releaseModel(JNIEnv* env,jobject obj);

static JNIEXPORT jboolean JNICALL setMinFaceSize(JNIEnv *env, jobject instance, jint minSize);

static JNIEXPORT jobject JNICALL detectFaceAlign(JNIEnv* env,jobject obj,
                                                 jbyteArray imgData,
                                                 jint width,jint height);

static JNIEXPORT jobject JNICALL detectFaceMax(JNIEnv* env,jobject obj,
                                               jbyteArray imgData,
                                               jint width,jint height);

static JNIEXPORT jobject JNICALL detectFaceNormal(JNIEnv* env,jobject obj,
                                                  jbyteArray imgData,
                                                  jint width,jint height);

static JNIEXPORT jboolean JNICALL setThreadsNumber(JNIEnv *env, jobject instance, jint threadsNumber);


static jobject  ToArraylist(JNIEnv*  env,
                             jobject obj,
                             std::vector<Bbox>& finalBbox,
                             std::vector<cv::Mat>& face_final);
};


#endif