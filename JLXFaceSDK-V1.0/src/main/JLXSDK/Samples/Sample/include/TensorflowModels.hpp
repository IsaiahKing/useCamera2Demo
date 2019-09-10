#ifndef JLXTENSORFLOWCUSTOM_HPP
#define JLXTENSORFLOWCUSTOM_HPP

#include <jni.h>
#include "tensorflow/lite/JlxTensorflow.h"

#ifdef LINUX
typedef pthread_mutex_t tflite_mutex_t;

#define TF_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#define tflite_mutex_init		pthread_mutex_init
#define tflite_mutex_lock		pthread_mutex_lock
#define tflite_mutex_unlock	    pthread_mutex_unlock
#define tflite_mutex_destroy	pthread_mutex_destroy

#endif
typedef enum {
    JLX_TF_STATUS_OK = 0,
    JLX_TF_STATUS_FAIL =1
}JLX_TF_STATUS;
using namespace tflite;
namespace JLX{
    namespace FC{
        /**
         * custom tflite class ,include  function (load model, invoke, get model output size )
         */
        class TFModels{
        public:
            TFModels();
            ~TFModels();
            /**
             * 初始化
             * @param env
             * @param jclzz
             * @param path 　传入 model 文件路径
             * @return if return 0,then success
             */
            long    init(JNIEnv* env,jobject jclzz,const std::string& path);
            /**
             *
             * @param fc6_input
             * @param fc_out
             * @return
             */
            int     run(float* fc6_input,std::vector<float>& fc_out);
            /**
             *
             * @return
             */
            int     GetOutPutSize();
        private:
            LoadTFLite TFLite;
            tflite_mutex_t mlite_mutex_t;
            const std::string TAG = "TFModels";
        };
    };
};

#endif