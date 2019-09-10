#include "jni_jlx_util.hpp"


//add tensorflowlite ...
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

const char kIllegalArgumentException[] = "java/lang/IllegalArgumentException";
const char kIllegalStateException[] = "java/lang/IllegalStateException";
const char kNullPointerException[] = "java/lang/NullPointerException";
const char kIndexOutOfBoundsException[] = "java/lang/IndexOutOfBoundsException";
const char kUnsupportedOperationException[] =
        "java/lang/UnsupportedOperationException";

class JLXJNIFlatBufferVerifier : public tflite::TfLiteVerifier {
public:
    bool Verify(const char* data, int length,
                tflite::ErrorReporter* reporter) override {
        if (!VerifyModel(data, length)) {
            reporter->Report("The model is not a valid Flatbuffer file");
            return false;
        }
        return true;
    }
    bool VerifyModel(const void* buf, size_t len) {
        flatbuffers::Verifier verifier(static_cast<const uint8_t*>(buf), len);
        return tflite::VerifyModelBuffer(verifier);
    }
};



// jni  <--> c/c++ string(const char*)
std::string JLX::util::ConvertJString(JNIEnv* env,jobject obj,jstring str) {
    const jsize len = env->GetStringUTFLength(str);
    const char* strChars = env->GetStringUTFChars(str, (jboolean *)0);

    std::string Result(strChars, len);
    env->ReleaseStringUTFChars(str, strChars);
    return Result;
}

jstring JLX::util::chartoJstring(JNIEnv* env, const char* pat) {
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

//记得释放内存...
char* JLX::util::jstringToChar(JNIEnv* env,jobject obj, jstring jstr) {
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("UTF-8");
    jmethodID   mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char*) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    env->DeleteLocalRef(clsstring);
    return rtn;
}

void JLX::util::byteArray2bgr(JNIEnv* env,jobject obj,
                   jbyteArray imgData,
                   cv::Mat& m_bgr){

    //byte[] yuv ===> b_g_r
    jboolean        isCopy;
    jsize dataLength = env->GetArrayLength(imgData);
    if(dataLength <= 0){
        return ;
    }
    //check size ...
#if DEBUG_OUT_INFO
    LOG(INFO) << "data length : "<< dataLength;
#endif
    jbyte *buffer = env->GetByteArrayElements(imgData, &isCopy);
    if(env->ExceptionCheck()){
        LOG(ERROR) << "ExceptionCheck true";
        return ;
    }
    if(buffer != nullptr) {
        cv::Mat m_yuv(face_height + face_width / 2, face_width, CV_8UC1, (unsigned char *)buffer);
        cv::cvtColor(m_yuv, m_bgr, CV_YUV420sp2BGR);
    }
    env->ReleaseByteArrayElements(imgData, buffer, JNI_ABORT);
}

namespace JLX {
    namespace lite {
        std::unique_ptr<OpResolver> CreateOpResolver() {  // NOLINT
            return std::unique_ptr<tflite::ops::builtin::BuiltinOpResolver>(
                    new tflite::ops::builtin::BuiltinOpResolver());
        }

        void ThrowException(JNIEnv* env, const char* clazz, const char* fmt, ...) {
            va_list args;
            va_start(args, fmt);
            const size_t max_msg_len = 512;
            auto* message = static_cast<char*>(malloc(max_msg_len));
            if (message && (vsnprintf(message, max_msg_len, fmt, args) >= 0)) {
                env->ThrowNew(env->FindClass(clazz), message);
            } else {
                env->ThrowNew(env->FindClass(clazz), "");
            }
            if (message) {
                free(message);
            }
            va_end(args);
        }

        BufferErrorReporter::BufferErrorReporter(JNIEnv* env, int limit) {
            buffer_ = new char[limit];
            if (!buffer_) {
                ThrowException(env, kNullPointerException,
                               "Internal error: Malloc of BufferErrorReporter to hold %d "
                               "char failed.",
                               limit);
                return;
            }
            buffer_[0] = '\0';
            start_idx_ = 0;
            end_idx_ = limit - 1;
        }

        BufferErrorReporter::~BufferErrorReporter() { delete[] buffer_; }

        int BufferErrorReporter::Report(const char* format, va_list args) {
            int size = 0;
            if (start_idx_ < end_idx_) {
                size = vsnprintf(buffer_ + start_idx_, end_idx_ - start_idx_, format, args);
            }
            start_idx_ += size;
            return size;
        }

        const char* BufferErrorReporter::CachedErrorMessage() { return buffer_; }

        //custom funtion
        long createErrorReporter(JNIEnv* env, jclass clazz, int size) {
            BufferErrorReporter* error_reporter =
                    new BufferErrorReporter(env, size);
            return reinterpret_cast<long>(error_reporter);
        }

        BufferErrorReporter* convertLongToErrorReporter(JNIEnv* env, long handle) {
            if (handle == 0) {
                ThrowException(env, kIllegalArgumentException,
                               "Internal error: Invalid handle to ErrorReporter.");
                return nullptr;
            }
            return reinterpret_cast<BufferErrorReporter*>(handle);
        }

        tflite::Interpreter* convertLongToInterpreter(JNIEnv* env, long handle) {
            if (handle == 0) {
                ThrowException(env, kIllegalArgumentException,
                               "Internal error: Invalid handle to Interpreter.");
                return nullptr;
            }
            return reinterpret_cast<tflite::Interpreter*>(handle);
        }

        tflite::FlatBufferModel* convertLongToModel(JNIEnv* env, long handle) {
            if (handle == 0) {
                ThrowException(env, kIllegalArgumentException,
                               "Internal error: Invalid handle to model.");
                return nullptr;
            }

            return reinterpret_cast<tflite::FlatBufferModel*>(handle);
        }

        void numThreads(JNIEnv* env,
                        jclass  clazz,
                        long    handle,
                        int     num_threads){

            tflite::Interpreter* interpreter = convertLongToInterpreter(env, handle);
            if (interpreter == nullptr) return;
            interpreter->SetNumThreads(num_threads);
        }

        void useNNAPI(JNIEnv* env,
                      jclass clazz,
                      long handle,
                      bool state) {
            tflite::Interpreter* interpreter = convertLongToInterpreter(env, handle);
            if (interpreter == nullptr) return;
            interpreter->UseNNAPI(state);
        }

        long create_model(JNIEnv* env, jclass clazz, string model_file, long error_handle){
            BufferErrorReporter* error_reporter =
                    convertLongToErrorReporter(env, error_handle);
            if (error_reporter == nullptr) return 0;

            std::unique_ptr<tflite::TfLiteVerifier> verifier;
            verifier.reset(new JLXJNIFlatBufferVerifier());

            auto model = tflite::FlatBufferModel::VerifyAndBuildFromFile(
                    model_file.c_str(), verifier.get(), error_reporter);
            if (!model) {
                ThrowException(env, kIllegalArgumentException,
                               "Contents of %s does not encode a valid "
                               "TensorFlowLite model: %s",
                               model_file.c_str(), error_reporter->CachedErrorMessage());

                return 0;
            }

            return reinterpret_cast<long>(model.release());
        }

        long createInterpreter(JNIEnv* env, jclass clazz, long model_handle, long error_handle,
                               int num_threads){

            tflite::FlatBufferModel* model = convertLongToModel(env, model_handle);
            if (model == nullptr) return 0;

            BufferErrorReporter* error_reporter =
                    convertLongToErrorReporter(env, error_handle);
            if (error_reporter == nullptr) return 0;

            auto resolver = CreateOpResolver();

            std::unique_ptr<tflite::Interpreter> interpreter;
            TfLiteStatus status = tflite::InterpreterBuilder(*model, *(resolver.get()))(
                    &interpreter, num_threads);
            if (status != kTfLiteOk) {
                ThrowException(env, kIllegalArgumentException,
                               "Internal error: Cannot create interpreter: %s",
                               error_reporter->CachedErrorMessage());
                return 0;
            }
            printf("%d ", static_cast<int>(interpreter->nodes_size()));
            return reinterpret_cast<long>(interpreter.release());
        }


    }  // namespace tflite
}  // namespace JLX