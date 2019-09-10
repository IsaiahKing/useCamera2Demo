#include "jni_classify.hpp"
#include "Samples.hpp"
#include "coffeecatch.h"
#include <android/log.h>
#include <facedetect/src/main/cpp/mtcnn.h>

using namespace std;
static std::unique_ptr<Samples> pFace = nullptr;

#define JNIREG_CLASS "com/jlx/face/FaceService"
#define JNIREG_CONVERT_CLASS "com/jlx/utils/Convert"

#ifdef __cplusplus
extern "C" {
#endif

// API to send one frame of image data to CNN engine and read back the classification results
#if 0
jobjectArray facePredict(JNIEnv* env,jobject obj,jbyteArray imgData) {
    jobjectArray ret;
    std::vector<std::string> predictionResults;
    signed char* buffer = env->GetByteArrayElements(imgData, 0);
    if (pFace != nullptr){
        //LOG(ERROR) << "step 1 jni facePredict" ;
        pFace->facePredict((unsigned char*)buffer, predictionResults);
        //LOG(ERROR) << "step 2 jni facePredict,from c/c++ out results" ;
        jstring value_local = chartoJstring(env,"");
        ret = (jobjectArray)env->NewObjectArray(5, env->FindClass("java/lang/String"), value_local);
        env->DeleteLocalRef(value_local);
        //LOG(ERROR) << "step 3 jni facePredict,NewObjectArray is OK" ;
        for(int i = 0; i < predictionResults.size(); i++)
        {
            jstring value = chartoJstring(env,predictionResults[i].c_str());//
            env->SetObjectArrayElement(ret, i,value);
            env->DeleteLocalRef(value);
        }
        //LOG(ERROR) << "jni step 4. jni facePredict,The assignment object" ;
        env->ReleaseByteArrayElements(imgData,buffer,0);//add 18-05-14
    }else{
        jstring value_local = chartoJstring(env,"");
        ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), value_local);
        env->DeleteLocalRef(value_local);
    }
    return(ret);
}
#endif

static JNIEXPORT jint JNICALL initEngine(JNIEnv* env,jobject obj,
                                        jint network, jint type,
                                        jstring packageResourcePath) {
    int  ret = 0;
    try {
        // 0 = gNet1, 1 = gNet2, "0" for FTDI USB 3.0 interface  /dev/sg0
        pFace.reset(new Samples(env,obj,network));

        string ResourcePath = JLX::util::ConvertJString(env, obj, packageResourcePath);
        pFace->SetPackageResourcePath(ResourcePath.c_str());//db file path
    }
    catch (std::bad_alloc& e){
        ELOG("InitEngining bad alloc thrown");
    } catch(...){
        ELOG("Unaware abnormal type");
        throw ;
    }
    if (pFace != nullptr)
    {
        //coef file and user configure file
        ret = pFace->JlxSDKStep(env,obj);
        if(JLX_JNI_INIT_OK != ret)
        {
            return ret;
        }
        // type 0: image, type 1: video, type 2: camera, type 3: face recognition
        if(JLX_JNI_INIT_OK == pFace->oneFramePredictInit(env,obj,type)){
#if DEBUG_OUT_INFO
            ALOG(" InitEngine End ----");
#endif
        }else{
#if DEBUG_OUT_ERROR
            ELOG(" InitEngine 3 failed ----");
#endif
        }
    }
    return ret;
}


#if 0
static  jboolean  Jlxregister(JNIEnv* env, jobject obj, cv::Mat& face, jstring name){
    jboolean result = FACE_ERROR;
    string   inputName = ConvertJString(env,obj,name);
    if(pFace != nullptr)
    {
        int ret = pFace->face_register(face,inputName.c_str());
        if(ret != 0){
#if DEBUG_OUT_ERROR
            LOG(ERROR) << "face register failed";
#endif
            return result;
        }
        result = FACE_CHECK;
    }
    else {
#if DEBUG_OUT_ERROR
        ELOG("pFace is empty");
#endif
    }
    return result;
}


static  jobject   Jlxpredict(JNIEnv* env, jobject obj, cv::Mat& face){
    jclass      f_jstr = env->FindClass("java/lang/String");
    if(env->ExceptionCheck()){
        env->DeleteLocalRef(f_jstr);
        ELOG("ExceptionCheck true");
        return nullptr;
    }
    if(pFace != nullptr) {
        std::vector<std::string> personinfo;
        pFace->facePredict(face, personinfo);

        if(personinfo.size() > 0) {//说明 有预测到数据
            jclass list_cls = env->FindClass("java/util/ArrayList");
            if(list_cls == nullptr) {
#if DEBUG_OUT_ERROR
                LOG(ERROR) << "Not find class about ArrayList";
#endif
                env->DeleteLocalRef(list_cls);
                return nullptr;
            }
            else {
                jmethodID list_costruct = env->GetMethodID(list_cls, "<init>", "()V");
                if (list_costruct != nullptr) {
                    jobject list_obj = env->NewObject(list_cls, list_costruct);
                    if (list_obj != nullptr) {
                        jmethodID list_add = env->GetMethodID(list_cls, "add",
                                                              "(Ljava/lang/Object;)Z");
                        for (int i = 0; i < personinfo.size(); ++i) {
                            jstring value = chartoJstring(env, personinfo.at(i).c_str());
                            if (value == nullptr) {
                                break;
                            }
                            env->CallBooleanMethod(list_obj, list_add, value);
                            env->DeleteLocalRef(value);
                        }
                        env->DeleteLocalRef(f_jstr);
                        return list_obj;
                    }else{
#if DEBUG_OUT_ERROR
                        LOG(ERROR) << "list obj is null";
#endif
                        env->DeleteLocalRef(list_obj);
                        return nullptr;
                    }
                }else{
#if DEBUG_OUT_ERROR
                    LOG(ERROR) << "list costruct is null";
#endif
                    env->DeleteLocalRef(list_cls);
                    return nullptr;
                }
            }
        }
    }
    env->DeleteLocalRef(f_jstr);

    return nullptr;
}
#endif


//------------------------------------------------------------------------
static JNIEXPORT jint JNICALL JLXFaceRegister(JNIEnv* env, jobject obj,
                                             jbyteArray imgData,
                                             jstring name) {


    cv::Mat     m_bgr;
    JLX::util::byteArray2bgr(env,obj,imgData,m_bgr);
    //check img size ...

    string inputName = JLX::util::ConvertJString(env,obj,name);

    if (pFace != nullptr && (!m_bgr.empty())) {
        int ret = pFace->face_register(m_bgr,inputName.c_str());
        if(ret != RegisterOK){
            LOG(ERROR) << "face register failed,error code : " << ret;
            return RegisterFail;
        }
    }
    else {
#if DEBUG_OUT_ERROR
        ELOG("pFace is empty");
#endif
    }
    return RegisterOK;
}


/***
 * 返回 ArrayList<String>
 */
static JNIEXPORT jobject JNICALL JLXFacePredict(JNIEnv* env, jobject obj,
                                    jbyteArray imgData){
    COFFEE_TRY() {
        cv::Mat     m_bgr;
        JLX::util::byteArray2bgr(env,obj,imgData,m_bgr);

        jclass f_jstr = env->FindClass("java/lang/String");
        if(env->ExceptionCheck()){
            env->DeleteLocalRef(f_jstr);
            ELOG("ExceptionCheck true");
            return nullptr;
        }
        if (pFace != nullptr && (!m_bgr.empty()))
        {
            std::vector<std::string> personinfo;

            pFace->facePredict(m_bgr, personinfo);

            if(personinfo.size() > 0) {//说明 有预测到数据
                jclass list_cls = env->FindClass("java/util/ArrayList");
                if(list_cls == nullptr) {
#if DEBUG_OUT_ERROR
                    LOG(ERROR) << "Not find class about ArrayList";
#endif
                    env->DeleteLocalRef(list_cls);
                    return nullptr;
                }
                else {
                    jmethodID list_costruct = env->GetMethodID(list_cls, "<init>", "()V");
                    if (list_costruct != nullptr) {
                        jobject list_obj = env->NewObject(list_cls, list_costruct);
                        if (list_obj != nullptr) {
                            jmethodID list_add = env->GetMethodID(list_cls, "add",
                                                                  "(Ljava/lang/Object;)Z");
                            for (int i = 0; i < personinfo.size(); ++i) {
                                jstring value = JLX::util::chartoJstring(env, personinfo.at(i).c_str());
                                if (value == nullptr) {
                                    break;
                                }
                                env->CallBooleanMethod(list_obj, list_add, value);
                                env->DeleteLocalRef(value);
                            }
                            env->DeleteLocalRef(f_jstr);

                            return list_obj;
                        }else{
                            LOG(ERROR) << "list obj is null";
                            env->DeleteLocalRef(list_obj);
                            return nullptr;
                        }
                    }else{
                        LOG(ERROR) << "list costruct is null";
                        env->DeleteLocalRef(list_cls);
                        return nullptr;
                    }
                }
            }
        }
        env->DeleteLocalRef(f_jstr);
    }
    COFFEE_CATCH()
    {
        /** Caught a signal. **/
        const char*const message = coffeecatch_get_message();
//        fprintf(stderr, "**FATAL ERROR: %s\n", message);
        LOG(ERROR) << "**FATAL ERROR:" << message;
    }
    COFFEE_END();
    return nullptr;
}


//得到人脸特征
static JNIEXPORT jfloatArray JNICALL JLXGetFaceFeature(JNIEnv* env,jobject obj,
                            jbyteArray imgData){

    cv::Mat m_bgr;
    JLX::util::byteArray2bgr(env,obj,imgData,m_bgr);
#if DEBUG_OUT_INFO
    LOG(INFO) << "jni feat length  1";
#endif
    if(pFace != nullptr && !m_bgr.empty()){
        float *facefeat  = pFace->getFaceFeature(m_bgr);
        int    feat_len  = pFace->GetFeatVectorLen();
        jfloatArray facefeatobj;
#if DEBUG_OUT_INFO
        LOG(INFO) << "jni feat length 2_   "<< feat_len;
#endif
        if(facefeat != nullptr){
            jsize facefeatsize = feat_len * sizeof(float);
            facefeatobj = env->NewFloatArray(facefeatsize);
            env->SetFloatArrayRegion(facefeatobj,0,facefeatsize,facefeat);
#if DEBUG_OUT_INFO
            LOG(ERROR) << "jni feat length 3   "<< facefeatsize;
#endif
            return facefeatobj;
        }
    }
    return nullptr;
}


//图片比对,输出置信度
static JNIEXPORT jfloat JNICALL JLXGetTwoFaceSimilar(JNIEnv* env,jobject obj,
            jbyteArray imgData1,jbyteArray imgData2){


#if DEBUG_OUT_INFO
    LOG(INFO) << "jni feat length  1";
#endif
    if(pFace != nullptr){
#if DEBUG_OUT_INFO
        LOG(INFO) << "jni feat length 2_   ";
#endif
        if(pFace != nullptr){
            float  s = pFace->compare2((unsigned char*)imgData1,(unsigned char*)imgData2);
#if DEBUG_OUT_INFO
            LOG(ERROR) << "jni feat length 3   ";
#endif
            return s;
        }
    }
    return 0.f;
}


//特征值比对
static JNIEXPORT jfloat JNICALL JLXGetTwoFeatureSimilar(JNIEnv* env,jobject obj,
                                                     jfloatArray feat1,
                                                        jfloatArray feat2){


#if DEBUG_OUT_INFO
    LOG(INFO) << "jni feat length  1";
#endif
    if(pFace != nullptr){
        jboolean isCopy;
        jsize dataLength1 = env->GetArrayLength(feat1);
        jsize dataLength2 = env->GetArrayLength(feat2);

        if(dataLength1 != dataLength2){
#if DEBUG_OUT_ERROR
            LOG(ERROR)<< "Two feat  not  length ";
#endif
            return 0.0f;
        }

        float *facefeat1  = env->GetFloatArrayElements(feat1,&isCopy);
        float *facefeat2  = env->GetFloatArrayElements(feat2,&isCopy);
#if DEBUG_OUT_INFO
        LOG(INFO) << "jni feat     ";
#endif
        float  s = 0;
        if(pFace != nullptr && facefeat1 != nullptr && facefeat2 != nullptr){

            s = pFace->TwoSimilar_C(facefeat1,facefeat2);
#if DEBUG_OUT_INFO
            LOG(INFO) << "jni Two Feature  Similar   "<< s;
#endif
        }
        env->ReleaseFloatArrayElements(feat1,facefeat1,JNI_FALSE);
        env->ReleaseFloatArrayElements(feat2,facefeat2,JNI_FALSE);
        return s;
    }
    return 0.f;
}


#if 0
static JNIEXPORT jobjectArray JNICALL oneFramePredict(JNIEnv* env, jobject obj,
                                                      jbyteArray imgData){
    jobjectArray ret;
    string predictResults[5];
    jclass jstr = env->FindClass("java/lang/String");
    signed char* buffer = env->GetByteArrayElements(imgData, 0);
    if (pFace != nullptr){
        pFace->oneFramePredict((unsigned char*)buffer, predictResults);
        jstring value_local = chartoJstring(env,"");
        ret = (jobjectArray)env->NewObjectArray(5, jstr, value_local);
        env->DeleteLocalRef(value_local);
        for(int i = 0; i < 2; i++)
        {
            jstring value = chartoJstring(env,predictResults[i].c_str());
            env->SetObjectArrayElement(ret, i,value);
            env->DeleteLocalRef(value);
        }
        env->ReleaseByteArrayElements(imgData,buffer,0);//add 18-05-14
    }else{
        jstring value_local = chartoJstring(env,"");
        ret = (jobjectArray)env->NewObjectArray(1, jstr, value_local);
        env->DeleteLocalRef(value_local);
    }
    env->DeleteLocalRef(jstr);
    return(ret);
}
#endif


static JNIEXPORT jobjectArray JNICALL JLXGetUserList(JNIEnv* env,jobject obj) {
    jobjectArray ret;
    std::vector<std::string> usernameList;
    jclass  f_jstr = env->FindClass("java/lang/String");
    jint    base_num = 1;
    if (pFace != nullptr)
    {
        try {
            int num = pFace->GetSQUserNum();
            if (num > 0) {
                usernameList.clear();
                pFace->SetSQUserList(usernameList);
            }
            else {
                LOG(ERROR) << "get User Num is 0 ";
                ret = (jobjectArray)env->NewObjectArray(base_num, f_jstr, 0);
                env->DeleteLocalRef(f_jstr);
                return nullptr;
            }
            ret = (jobjectArray)env->NewObjectArray(num, f_jstr, 0);

            for(int i = 0; i < num; ++i)
            {
                string &tmp_value = usernameList.at(i);
                jstring value = JLX::util::chartoJstring(env,tmp_value.c_str());
                env->SetObjectArrayElement(ret, i, value);
                env->DeleteLocalRef(value);
            }
            env->DeleteLocalRef(f_jstr);
        }catch(exception e){
            throw "Throw A Exception about face UserList";
        }
    }
    else
    {
        env->DeleteLocalRef(f_jstr);
        return nullptr;
    }
    return ret;
}


static JNIEXPORT jint JNICALL JLXDelUserName(JNIEnv* env,jobject obj,
                                            jstring name){
    char* delName = JLX::util::jstringToChar(env,obj,name);
    int   ret = -1;
    if (pFace != nullptr){
        try {
            ret = pFace->DelName(delName);
        }catch (exception e){
            throw "Throw A Exception about del User Name";
        }
    }
    if (delName != NULL){
        free(delName);
    }
    return ret;
}


static JNIEXPORT void JNICALL JLXUnInitEngine(JNIEnv* env,jobject obj){
    pFace.release();
}


static JNIEXPORT jintArray JNICALL yuv420sp2rgb(JNIEnv* env,jobject obj,
                                          jbyteArray yuv420sp,
                                          int width,
                                          int height) {
    int frameSize = width * height;
    int *des = new int[frameSize];

    jbyte* source = env->GetByteArrayElements(yuv420sp,0);
    int brightness_factor = 1000;

    brightness_factor = 1192 * brightness_factor / 1000;

    for (int j = 0, yp = 0; j < height; j++) {
        int uvp = frameSize + (j >> 1) * width, u = 0, v = 0;
        for (int i = 0; i < width; i++, yp++) {
            int y = (0xff & ((int) source[yp])) - 16;
            if (y < 0)
                y = 0;
            if ((i & 1) == 0) {
                v = (0xff & source[uvp++]) - 128;
                u = (0xff & source[uvp++]) - 128;
            }

            int y1192 = brightness_factor * y; //1192 * y * brightness_factor /1000;
            int r = (y1192 + 1634 * v);
            int g = (y1192 - 833 * v - 400 * u);
            int b = (y1192 + 2066 * u);

            if (r < 0)
                r = 0;
            else if (r > 262143)
                r = 262143;
            if (g < 0)
                g = 0;
            else if (g > 262143)
                g = 262143;
            if (b < 0)
                b = 0;
            else if (b > 262143)
                b = 262143;

            des[yp] = 0xff000000 | ((r << 6) & 0xff0000) | ((g >> 2) & 0xff00) | ((b >> 10) & 0xff);
        }
    }

    jintArray result = env->NewIntArray(frameSize);
    env->SetIntArrayRegion(result, 0, frameSize, des);
    env->ReleaseByteArrayElements(yuv420sp, source, 0);

    delete []des;
    return result;
}

//动态注册jni 函数..  1.定义函数映射表
static  JNINativeMethod method_table[] = {
        {"initEngine","(IILjava/lang/String;)I",(jint*)initEngine},
        {"JLXgetUserList","()[Ljava/lang/String;",(jobjectArray*)JLXGetUserList},
        {"JLXfaceregister","([BLjava/lang/String;)I",(jint*)JLXFaceRegister},
        {"JLXfacePredict","([B)Ljava/util/ArrayList;",(jobject*)JLXFacePredict},
        {"JLXdelUserName","(Ljava/lang/String;)I",(jint*)JLXDelUserName},
        {"initdetectModel","(Ljava/lang/String;)I",(jint*)JNIDetect::InitDetectModel},
        {"detectFace","([BII)Ljava/util/ArrayList;",(jobject*)JNIDetect::detectFaceNormal},
        {"detectMaxFace","([BII)Ljava/util/ArrayList;",(jobject*)JNIDetect::detectFaceMax},
        {"detectFaceAlign","([BII)Ljava/util/ArrayList;",(jobject*)JNIDetect::detectFaceAlign},
        {"setMinFaceSize","(I)Z",(jboolean*)JNIDetect::setMinFaceSize},
        {"setThreadsNumber","(I)Z",(jboolean*)JNIDetect::setThreadsNumber},
        {"releseModel","()Z",(void*)JNIDetect::releaseModel},
        {"JLXunInitEngine","()V",(void*)JLXUnInitEngine},
        {"JLXGetFaceFeature","([B)[F",(jfloatArray*)JLXGetFaceFeature},
        {"JLXGetTwoFaceSimilar","([B[B)F",(jfloat*)JLXGetTwoFaceSimilar},
        {"JLXGetTwoFeatureSimilar","([F[F)F",(jfloat*)JLXGetTwoFeatureSimilar}
};


static  JNINativeMethod convert_method_table[] = {
        {"yuv420sp2rgb","([BII)[I",(jintArray*)yuv420sp2rgb}
};


static int registerNativeMethods(JNIEnv* env,
                                 const char* className,
                                 JNINativeMethod* gMethods,
                                 int numMethods) {
    jclass clazz = env->FindClass(className);//调用了findclass
    //找到对应类名..
    if (clazz == nullptr)
    {
        env->DeleteLocalRef(clazz);//释放引用
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
    {
        env->DeleteLocalRef(clazz);
        return JNI_FALSE;
    }
    env->DeleteLocalRef(clazz);
    return JNI_TRUE;
}


static int registerNatives(JNIEnv* env) {
    if (0 == registerNativeMethods(env,JNIREG_CLASS, method_table, sizeof(method_table)/sizeof(method_table[0])))
    {
        return JNI_FALSE;
    }
    if(0 == registerNativeMethods(env,JNIREG_CONVERT_CLASS,convert_method_table,
                                 sizeof(convert_method_table)/sizeof(convert_method_table[0]))){
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env    = nullptr;
    jint    result;

    if (vm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }

    if (!registerNatives(env)) //注册
    {
        return -1;
    }

    result = JNI_VERSION_1_6;

    return result;
}


JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
    pFace.release();
    //ALOG("JNI_OnUnload");
}

#ifdef __cplusplus
}
#endif