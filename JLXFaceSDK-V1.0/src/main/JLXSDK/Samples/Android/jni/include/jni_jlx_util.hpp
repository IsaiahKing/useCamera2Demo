#ifndef __JNI_JLXUTIL_HPP
#define __JNI_JLXUTIL_HPP


#include <jni.h>
#include <string>

#include "Samples.hpp"
#include "tensorflow/lite/error_reporter.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/optional_debug_tools.h"

extern const char kIllegalArgumentException[];
extern const char kIllegalStateException[];
extern const char kNullPointerException[];
extern const char kIndexOutOfBoundsException[];
extern const char kUnsupportedOperationException[];

namespace JLX{
    namespace util{
        std::string  ConvertJString(JNIEnv* env,jobject obj,jstring str);
        jstring      chartoJstring(JNIEnv* env, const char* pat);
        char*        jstringToChar(JNIEnv* env,jobject obj, jstring jstr);

        void         byteArray2bgr(JNIEnv* env,jobject obj,
                           jbyteArray imgData,
                           cv::Mat& m_bgr);

        static const int face_width  = 224;
        static const int face_height = 224;
    };

    namespace lite{
        void ThrowException(JNIEnv* env, const char* clazz, const char* fmt, ...);

        class BufferErrorReporter : public ErrorReporter {
        public:
            BufferErrorReporter(JNIEnv* env, int limit);
            virtual ~BufferErrorReporter();
            int Report(const char* format, va_list args) override;
            const char* CachedErrorMessage();

        private:
            char* buffer_;
            int start_idx_ = 0;
            int end_idx_ = 0;
        };

        //
        long createErrorReporter(JNIEnv* env, jclass clazz, int size);
        BufferErrorReporter* convertLongToErrorReporter(JNIEnv* env, long handle);
        tflite::Interpreter* convertLongToInterpreter(JNIEnv* env, long handle);
        tflite::FlatBufferModel* convertLongToModel(JNIEnv* env, long handle);
        void numThreads(JNIEnv* env,
                        jclass  clazz,
                        long    handle,
                        int     num_threads);
        void useNNAPI(JNIEnv* env,
                      jclass clazz,
                      long handle,
                      bool state);
        long create_model(JNIEnv* env, jclass clazz, string model_file, long error_handle);
        long createInterpreter(JNIEnv* env, jclass clazz, long model_handle, long error_handle,
                               int num_threads);
    };

};
#endif