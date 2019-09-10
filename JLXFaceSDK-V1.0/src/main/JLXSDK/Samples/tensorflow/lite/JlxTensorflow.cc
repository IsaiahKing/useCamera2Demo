#include "JlxTensorflow.h"
#include "tensorflow/lite/minimal_logging.h"
namespace tflite{
  //added　　2019-07-25
  class JLXFlatBufferVerifier : public tflite::TfLiteVerifier {
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
  LoadTFLite::LoadTFLite() {
  }

  LoadTFLite::LoadTFLite(const std::string& path) {
      model_path = path;
  }
  LoadTFLite::~LoadTFLite() {
  }

  long LoadTFLite::build(JNIEnv* env,jobject jclzz,std::string path){
      if(path.length() > 0){
          model_path = path;
      }
      //model 路径以及文件是否存在...
      verifier.reset(new JLXFlatBufferVerifier());

      //校验并且加载文件
      model = tflite::FlatBufferModel::VerifyAndBuildFromFile(model_path.c_str(),verifier.get(),&error_reporter);//
      if(model == nullptr)return -2;

      resolver = std::unique_ptr<tflite::ops::builtin::BuiltinOpResolver>(
              new tflite::ops::builtin::BuiltinOpResolver());
      if(resolver == nullptr)return -3;

      builder = std::unique_ptr<tflite::InterpreterBuilder>(
              new tflite::InterpreterBuilder(*model, *(resolver.get())));

      if(builder == nullptr)return -4;

      TfLiteStatus status_tmp = (*builder.get())(&interpreter,num_threads);
      if(status_tmp == kTfLiteOk){
          PrintInterpreterState(interpreter.get());
          mstatus = status_tmp;
          if(isdebug){
              TFLITE_LOG_PROD_ONCE(TFLITE_LOG_INFO, "LINE : %d ,%s GetAllowFp16PrecisionForFp32 : %d",__LINE__,TAG,interpreter->GetAllowFp16PrecisionForFp32());
          }
          if (isdebug) {
              if(interpreter.get() == nullptr){
                  TFLITE_LOG_PROD_ONCE(TFLITE_LOG_ERROR, "LINE : %d ,%s interpreter failed",__LINE__,TAG);
                  return -5;
              }
          }

          if (isdebug) {
              TFLITE_LOG_PROD_ONCE(TFLITE_LOG_INFO, "LINE : %d , %s  nodes size: %d",__LINE__,TAG,interpreter->nodes_size() );
              TFLITE_LOG_PROD_ONCE(TFLITE_LOG_INFO, "LINE : %d , %s  inputs: %d",__LINE__,TAG,interpreter->inputs().size() );
              TFLITE_LOG_PROD_ONCE(TFLITE_LOG_INFO, "LINE : %d , %s  input(0) name: %s",__LINE__,TAG, interpreter->GetInputName(0)  );
          }
          resolver.release();
          builder.release();
      }
      return 0;
  }


//fc lite ,input size
float* LoadTFLite::inferential(float* fc6,std::vector<float>& v_fc7out){
    if(mstatus != kTfLiteOk)
        return nullptr;
    if(model == nullptr)return nullptr;
    if(fc6 == nullptr)return nullptr;
    //开始计时
   struct timeval start_once_time, stop_once_time;
   struct timeval start_time, stop_time;
   if(istest){
       gettimeofday(&start_once_time, nullptr);
   }
   resolver = std::unique_ptr<tflite::ops::builtin::BuiltinOpResolver>(new tflite::ops::builtin::BuiltinOpResolver());
   if(resolver == nullptr)return nullptr;

   builder = std::unique_ptr<tflite::InterpreterBuilder>(new tflite::InterpreterBuilder(*model, *resolver));
   if(builder == nullptr)return nullptr;
   TfLiteStatus ok = (*builder.get())(&interpreter);
   if(ok == kTfLiteOk){
       if(isdebug){
           // LOG(INFO) << " interpreter kTfLiteOk \n";
           PrintInterpreterState(interpreter.get());
       }

       if(istest){
           gettimeofday(&start_time, nullptr);
       }

       int   input           = interpreter->inputs()[0];
       const std::vector<int> inputs = interpreter->inputs();
       const std::vector<int> outputs = interpreter->outputs();

       if(isdebug){
           // LOG(INFO) << TAG <<  " number of inputs: " << inputs.size() << "\n";
           // LOG(INFO) << TAG <<  " number of outputs: " << outputs.size() << "\n";
       }

       if(kTfLiteOk != interpreter->AllocateTensors()){
           // LOG(FATAL) << "LINE : " << __LINE__  << " AllocateTensors Failed"<< "\n";
           exit(1);
       }else{
           // LOG(INFO) << "LINE : " << __LINE__   << " AllocateTensors ok" << "\n";
       }

       if (isdebug) PrintInterpreterState(interpreter.get());
       TfLiteIntArray* dims = interpreter->tensor(input)->dims;
       int wanted_height   = dims->data[1];
       int wanted_width    = dims->data[2];
       int wanted_channels = dims->data[3];

       if(isdebug){
           // LOG(INFO) << "wanted_height : "   << wanted_height << "\n";
           // LOG(INFO) << "wanted_width : "    << wanted_width << "\n";
           // LOG(INFO) << "wanted_channels : " << wanted_channels  << "\n";
       }
       if(isdebug){
           switch (interpreter->tensor(input)->type) {
               case kTfLiteFloat32:
                   input_floating = true;
                   // LOG(INFO) << " use TfLiteFloat32  " << "\n";
                   break;
               case kTfLiteUInt8:
                   // LOG(INFO) << " input use kTfLiteUInt8  " << "\n";
                   break;
               default:
                   // LOG(FATAL) << "cannot handle input type "
                   //            << interpreter->tensor(input)->type << " yet";
                   exit(-1);
           }
       }
       //check size feat_vector_len or wanted_height,only once
       //fill 7*7*512 data_size
       auto input_data = interpreter->typed_input_tensor<float>(0);//uint8_t  float
       for (int i = 0; i < feat_vector_len; i++) {
           input_data[i] = (fc6[i]);//static_cast<uint8_t>
       }
       //可 测试Invoke 耗时,loop_count 次数
       for (int i = 0; i < loop_count; i++) {
           // if (interpreter->Invoke() != kTfLiteOk) {
           //     LOG(FATAL) << "Failed to invoke tflite!\n";
           // }else{
           //     LOG(INFO) << " invoke ok !\n";
           // }
       }
       if(istest){
           gettimeofday(&stop_time, nullptr);
           // LOG(INFO) << "average time: "
           //           << (get_us(stop_time) - get_us(start_time)) / (loop_count * 1000)
           //           << " ms \n";
       }

       int output = interpreter->outputs()[0];
       if(isdebug){
           // LOG(INFO) << " output  size " << output  << "\n";
       }

       TfLiteIntArray* output_dims = interpreter->tensor(output)->dims;
       auto output_size  = output_dims->data[output_dims->size - 1];

       if(isdebug){
           // LOG(INFO) << " output_dims  size " <<  output_dims->data[1]  << "\n";
           // LOG(INFO) << " output_dims2  size " << output_dims->data[2]  << "\n";
           // LOG(INFO) << " output_dims3  size " << output_dims->data[3]  << "\n";
       }
       //check fc7output_size output_size
       auto outputdata = interpreter->typed_output_tensor<float>(0);
       for (size_t i = 0; i < output_size; i++) {
           v_fc7out.push_back((outputdata[i]));
       }
       if(isdebug){
           switch (interpreter->tensor(output)->type) {
               case kTfLiteFloat32:
                   // LOG(INFO) << " output_dims kTfLiteFloat32 \n";
                   break;
               case kTfLiteUInt8:
                   // LOG(INFO) << " output_dims kTfLiteUInt8 \n";
                   break;
               default:
                   // LOG(FATAL) << "cannot handle output type "
                   //            << interpreter->tensor(input)->type << " yet";
                   exit(-1);
           }
       }
   }
   if(istest){
       gettimeofday(&stop_once_time, nullptr);
       // LOG(INFO) << " process spend time: "
       //           << ((get_us(stop_once_time) - get_us(start_once_time)) / 1000)
       //           << " ms \n";
   }
    resolver.release();
    builder.release();
    return nullptr;
}
};//namespace tflite
