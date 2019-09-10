#include <logging.h>
#include "TensorflowModels.hpp"

namespace JLX{
    namespace FC{
        TFModels::TFModels():mlite_mutex_t(TF_MUTEX_INITIALIZER){
            tflite_mutex_init(&mlite_mutex_t,NULL);
        }
        TFModels::~TFModels(){
            tflite_mutex_destroy(&mlite_mutex_t);
        }

        long TFModels::init(JNIEnv* env,jobject jclzz,const std::string& path){
            return TFLite.build(env,jclzz,path);
        }

        int TFModels::run(float* fc6_input,std::vector<float>& fc_out){
            if(fc6_input != nullptr){
                tflite_mutex_lock(&mlite_mutex_t);
                TFLite.inferential(fc6_input,fc_out);
                tflite_mutex_unlock(&mlite_mutex_t);
                return JLX_TF_STATUS_OK;
            }
            return JLX_TF_STATUS_FAIL;
        }

        int TFModels::GetOutPutSize(){
            int input  = 0;
            int output = 0;
            TFLite.GetInputOutput(input,output);
            return output;
        }

    };
};



