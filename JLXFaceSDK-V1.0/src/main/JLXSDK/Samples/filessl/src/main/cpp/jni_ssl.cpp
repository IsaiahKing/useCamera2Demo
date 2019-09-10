#include <jni.h>
#include <string>
#include <zconf.h>
#include <android/log.h>
#include "verify.hpp"
#define JNISSL_CLASS "com/jenkolux/filessl/JlxEncrypt"

#ifdef __cplusplus
extern "C" {
#endif

std::string ConvertJString(JNIEnv* env,jobject obj,jstring str) {
    const jsize len = env->GetStringUTFLength(str);
    const char* strChars = env->GetStringUTFChars(str, (jboolean *)0);

    std::string Result(strChars, len);
    env->ReleaseStringUTFChars(str, strChars);
    return Result;
}

static jstring chartoJstring(JNIEnv* env, const char* pat) {
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray((jsize)strlen(pat));
    if(bytes == nullptr){
        return nullptr;
    }
    env->SetByteArrayRegion(bytes, 0, (jsize)strlen(pat), (jbyte*)pat);
    jstring encoding = env->NewStringUTF("UTF-8");//GB2312

    jstring result = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);

    env->DeleteLocalRef(bytes);
    env->DeleteLocalRef(strClass);

    return result;
}

static JNIEXPORT jstring JNICALL encode(JNIEnv* env,
                                        jobject instance,
                                        jstring str){
    std::string     m_str = ConvertJString(env,instance,str);
    char*     argv[] = {(char*)m_str.c_str(),(char*)"jk123456"};

    std::string result = ssl_main(argv);
    if(!result.empty()){
        return chartoJstring(env,result.c_str());
    }
    return NULL;
};

static  JNINativeMethod method_table[] = {
        {"encode","(Ljava/lang/String;)Ljava/lang/String;",(jstring *)encode}
};

static int registerNativeMethods(JNIEnv* env,
                                 const char* className,
                                 JNINativeMethod* gMethods,
                                 int numMethods) {
    jclass clazz = env->FindClass(className);//调用了findclass
    //找到对应类名..
    if (clazz == NULL)
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
    if (0 == registerNativeMethods(env,JNISSL_CLASS, method_table, sizeof(method_table)/sizeof(method_table[0])))
    {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env    = NULL;
    jint    result = -1;

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

#ifdef __cplusplus
}
#endif