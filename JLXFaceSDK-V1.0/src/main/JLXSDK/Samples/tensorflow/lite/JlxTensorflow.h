#ifndef __JLXTENSORFLOW_HPP
#define __JLXTENSORFLOW_HPP

#include <jni.h>
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/optional_debug_tools.h"

namespace tflite{
  class LoadTFLite{
  public:
      LoadTFLite();
      LoadTFLite(const std::string& path);
      ~LoadTFLite();
  public:
      // long     load(JNIEnv* env,long ret);
      long     build(JNIEnv* env,jobject jclzz,std::string path= nullptr);
      float*   inferential(float* fc6,std::vector<float>& fc_out);
      void      GetInputOutput(int& inputSize,int& outputSize);
  protected:
      StderrReporter error_reporter;
      std::unique_ptr<FlatBufferModel> model;

      std::unique_ptr<tflite::Interpreter>    interpreter;
      std::unique_ptr<tflite::TfLiteVerifier> verifier;
      std::unique_ptr<tflite::ops::builtin::BuiltinOpResolver> resolver;
      std::unique_ptr<tflite::InterpreterBuilder> builder;
  private:
      std::string model_path;
      int num_threads = 2;
      size_t feat_vector_len = 25088;
      size_t fc7output_size = 512;
      TfLiteStatus mstatus;
      const char* TAG = "LoadTFLite";
      bool   isdebug = true;
      bool   istest      = false;
      // bool   profiling = false;
      bool   input_floating = false;
      int    loop_count = 1;
      // float  input_mean = 127.5f;
      // float  input_std = 127.5f;
  };
};

#endif
