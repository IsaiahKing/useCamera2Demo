#include <zconf.h>
#include <facedetect/src/main/cpp/mtcnn.h>
#include <Android/jni/include/jni_classify.hpp>
#include "jni_detect.hpp"
#include "jni_jlx_util.hpp"

static unsigned long get_current_time(void) {
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (tv.tv_sec*1000000 + tv.tv_usec);
}

static std::unique_ptr<MTCNN> pTmcnn = nullptr;
static jboolean  detection_sdk_init_ok = JNI_FALSE;
const  static int minFaceNum = 120;

//初始化人脸检测模型
JNIEXPORT jint JNICALL JNIDetect::InitDetectModel(JNIEnv* env,jobject obj,
                                              jstring modelPath){

    std::string      jmodelPath      = JLX::util::ConvertJString(env,obj,modelPath);
    jsize            model_length    = env->GetStringLength(modelPath);
    if(model_length <= 0)
    {
        return  DETECT_MODEL_FILE_NO_EXIST;//This is empty path
    }
    try {
        if(pTmcnn == nullptr) {
            if(0 == access(jmodelPath.c_str(),F_OK)){

                pTmcnn.reset(new MTCNN(jmodelPath));
                if(pTmcnn != nullptr)
                {
                    pTmcnn->SetMinFace(minFaceNum);
                    return DETECT_OK;
                }else{
                    return DETECT_NO_POINT;
                }
            }else{
#if DEBUG_OUT_ERROR
                LOG(ERROR) << "detect model file ,no exist,please check out,again ";
#endif
                return DETECT_FILE_ACCESS_FAIL;
            }
        }else{
#if DEBUG_OUT_ERROR
            LOG(ERROR) << "Mtcnn Currently unavailable ";
#endif
            return  DETECT_STATUS_UNAVAILABLE;
        }
    }catch (exception e){
    }
    return -1;//init ok
}


//检测人脸
//[In] imgData 图像,width 宽  height 高 faceMode 模式 [Out] 人脸信息
JNIEXPORT jobject  JNICALL JNIDetect::detectFace(JNIEnv* env,jobject obj,
                            jbyteArray imgData,
                            jint width,jint height,
                            jint faceMode){
    jboolean isCopy;
    jbyte*   _yuv = env->GetByteArrayElements(imgData,&isCopy);
    if(_yuv == nullptr){
        return nullptr;
    }
    jsize dataLength = env->GetArrayLength(imgData);
    if(dataLength <= 0){
#if DEBUG_OUT_ERROR
        LOG(ERROR) << "data Length: " << dataLength;
#endif
        return nullptr;
    }
    if(width <= 0 || height <= 0){
#if DEBUG_OUT_ERROR
        LOG(ERROR) << "pre width: " << width << " height : " << height;
#endif
        return nullptr;
    }

    try {
        if(pTmcnn.get() != nullptr){
            cv::Mat     m_yuv(height + height/2,width,CV_8UC1,(unsigned char*)_yuv);
            cv::Mat     m_bgr;
            cvtColor(m_yuv,m_bgr,CV_YUV420sp2BGR);

            ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(m_bgr.data, ncnn::Mat::PIXEL_BGR2RGB, m_bgr.cols, m_bgr.rows);
#if OUTINFO_SPEND
            unsigned long start_time = get_current_time();
#endif
            std::vector<Bbox> finalBbox;
            finalBbox.clear();

            std::vector<cv::Mat> face_final_;
            face_final_.clear();

            if(faceMode == FACE_MAX){
                pTmcnn->detectMaxFace(ncnn_img, finalBbox);
            }
            else if(faceMode == FACE_NORMAL){
                pTmcnn->detect(ncnn_img, finalBbox);
            }else if(faceMode == FACE_ALIGN){
                pTmcnn->detectMaxWithAlign(ncnn_img, finalBbox,face_final_);
            }

            if(!finalBbox.empty()) {
#if OUTINFO_SPEND
                unsigned long finish_time = get_current_time();
                float total_time = static_cast<float> ((finish_time - start_time) / 1000);
                static int  icount = 0;
                icount++;
                static long lastfinishtime = 0;
                float interval_time = static_cast<float> (finish_time - lastfinishtime) / 1000;
                LOG(INFO) << "icount:  " << icount << " interval_time: " << interval_time;
                LOG(INFO) << " detect face  spend time " << total_time << " ms  ";
                lastfinishtime = finish_time;
#endif
            }
            else{
#if OUTINFO_SPEND
                static int iemptycount = 0;
                iemptycount++;
                LOG(INFO) << "spend iemptycount:  " << iemptycount;
#endif
            }
            env->ReleaseByteArrayElements(imgData,_yuv,JNI_ABORT);

            return ToArraylist(env,obj,finalBbox,face_final_);
        }else{
#if DEBUG_OUT_ERROR
            LOG(ERROR) << "pTmcnn object is nullptr ";
#endif
        }
    }catch (exception e){
    }
    return nullptr;
}

jobject  JNIDetect::ToArraylist(JNIEnv* env,
                             jobject obj,
                             std::vector<Bbox>& finalBbox,
                             std::vector<cv::Mat>& face_final){

    jclass list_cls = env->FindClass("java/util/ArrayList");
    if(list_cls == nullptr) {
#if DEBUG_OUT_ERROR
        LOG(ERROR) << "Not find class about ArrayList";
#endif
        env->DeleteLocalRef(list_cls);
        return nullptr;
    }
    else {
        jmethodID list_costruct = env->GetMethodID(list_cls,"<init>","()V");
        if(list_costruct != nullptr) {
            jobject list_obj = env->NewObject(list_cls,list_costruct);
            if(list_obj != nullptr) {
                jmethodID   list_add = env->GetMethodID(list_cls,"add","(Ljava/lang/Object;)Z");
                jclass      fjclazz = env->FindClass(JCLASS_PATH);

                if(fjclazz == nullptr){
#if DEBUG_OUT_ERROR
                    LOG(ERROR) << "Not find class JlxBox";
#endif
                    env->DeleteLocalRef(fjclazz);
                    return nullptr;
                }
                else {
#if DEBUG_OUT_INFO
                    LOG(INFO)  << "find a class about JlxBox";
#endif
                    jmethodID mid = env->GetMethodID(fjclazz, "<init>", "(IIII)V");
                    const int32_t num_box = static_cast<int32_t>(finalBbox.size());
                    if(mid != nullptr && (num_box > 0)) {

                        //构造 java 中 JlxBox 对象
                        jobject jlxbox_obj = env->NewObject(fjclazz , mid,
                                                            finalBbox.at(0).x1,finalBbox.at(0).y1,//left top
                                                            finalBbox.at(0).x2,finalBbox.at(0).y2);//right bottom
#if 0
                        static int icountimage=0;
                        char fname[50] = {0};
                        sprintf(fname,"/sdcard/%d.jpg",icountimage++);
                        cv::imshow(fname,face_final.at(0));

                        IplImage ipm = IplImage(face_final.at(0));
                        cvSaveImage(fname,&ipm,0);
#endif

                        //设置 face data
                        if(!face_final.empty()){
                            jfieldID  jfaceAlignFlag = env->GetFieldID(fjclazz,"alignface","I");
                            jfieldID  jfacefinal = env->GetFieldID(fjclazz,"face_final","[B");
                            if(jfacefinal == nullptr){
#if DEBUG_OUT_ERROR
                                LOG(ERROR)  << "jfacefinal not find";
#endif
                                return nullptr;
                            }

                            jbyteArray   faceBytesArray = (jbyteArray)env->GetObjectField(jlxbox_obj,jfacefinal);
                            cv::Mat     yuv_put;
                            cv::cvtColor(face_final.at(0),yuv_put,CV_BGR2YUV_I420);

                            if(jfaceAlignFlag == nullptr){
#if DEBUG_OUT_ERROR
                                LOG(ERROR)  << "jfaceAlignFlag not find";
#endif
                            }else{
                                env->SetIntField(jlxbox_obj,jfaceAlignFlag,1);
                            }
                            env->SetByteArrayRegion(faceBytesArray,0,JLX::util::face_width*JLX::util::face_height*3/2,(jbyte*)yuv_put.data);
                        }
                        //将对象添加  到 ArrayList 中
                        env->CallBooleanMethod(list_obj , list_add,jlxbox_obj);
                        env->DeleteLocalRef(jlxbox_obj);
                        env->DeleteLocalRef(fjclazz);
                        return list_obj;
                    }
                    else {
                        env->DeleteLocalRef(fjclazz);
                        env->DeleteLocalRef(list_obj);
                        return nullptr;
                    }
                }
            }
            else {
#if DEBUG_OUT_ERROR
                LOG(ERROR) << "constuct ArratList new object failed";
#endif
                env->DeleteLocalRef(list_obj);
                return nullptr;
            }
        }
        else {
#if DEBUG_OUT_ERROR
            LOG(ERROR) << "ListArray constuct method failed";
#endif
            return nullptr;
        }
    }
}

JNIEXPORT jboolean JNICALL JNIDetect::releaseModel(JNIEnv* env,jobject obj){
    jboolean result = 0;//JNI_FALSE
    if(pTmcnn != nullptr){
        pTmcnn.release();
    }else{
        return result;
    }
    return result;
}

//正常
JNIEXPORT jobject JNICALL JNIDetect::detectFaceNormal(JNIEnv* env,jobject obj,
                                                  jbyteArray imgData,
                                                  jint width,jint height){
    return detectFace(env,obj,imgData,width,height,FACE_NORMAL);
}

//只检测最大
JNIEXPORT jobject JNICALL JNIDetect::detectFaceMax(JNIEnv* env,jobject obj,
                                               jbyteArray imgData,
                                               jint width,jint height){
    return detectFace(env,obj,imgData,width,height,FACE_MAX);
}



JNIEXPORT jobject JNICALL JNIDetect::detectFaceAlign(JNIEnv* env,jobject obj,
                                                 jbyteArray imgData,
                                                 jint width,jint height){
    return detectFace(env,obj,imgData,width,height,FACE_ALIGN);
}

JNIEXPORT jboolean JNICALL JNIDetect::setMinFaceSize(JNIEnv *env, jobject instance, jint minSize) {
    jboolean result = JNI_FALSE;

    if(!detection_sdk_init_ok){
#if DEBUG_OUT_INFO
        LOG(WARNING) << "人脸检测 MTCNN模型SDK未初始化,直接返回";
#endif
        return result;
    }

    if(minSize <=20){
        minSize =20;//最小设置为20
    }
    result = JNI_TRUE;
    if(pTmcnn.get() != nullptr){
        pTmcnn->SetMinFace(minSize);
    }

    return result;
}

JNIEXPORT jboolean JNICALL JNIDetect::setThreadsNumber(JNIEnv *env, jobject instance, jint threadsNumber) {
    jboolean result = JNI_FALSE;

    if(!detection_sdk_init_ok || threadsNumber <= 0){
#if DEBUG_OUT_INFO
        LOG(WARNING) << "人脸检测 MTCNN模型SDK未初始化,直接返回";
#endif
        return result;
    }
    jint Number;
    if(threadsNumber > 8){
        Number = 2;
    }else{
        Number = threadsNumber;
    }
    result = JNI_TRUE;
    if(pTmcnn.get() != nullptr){
        pTmcnn->SetNumThreads(Number);
    }
    return result;
}