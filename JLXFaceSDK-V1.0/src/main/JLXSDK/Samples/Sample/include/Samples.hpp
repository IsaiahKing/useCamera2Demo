/***************************************************************
 * Samples.hpp
 * GTI2801 Sample code header file.
 *
 * Copyrights, Gyrfalcon technology Inc. 2019
 ***************************************************************/
#pragma once

#include <iomanip>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>

#include <stdio.h>
#include <stdlib.h>
#include "HandleFile.hpp"
#ifdef WIN32
#include <windows.h>
#endif
#include <time.h>
#include <cstring>

#include <functional>
#include "Classify.hpp"
#include "GTILib.h"
#include "JLXsqlite3.hpp"
#include "JLXDefine.hpp"
#include "Threadpool.hpp"
#include "ThreadQueue.hpp"

#ifdef LINUX
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <logging.h>
#include <fcntl.h>
#include <json/value.h>
#include <json/reader.h>
#include "TensorflowModels.hpp"



#define scanf_s scanf
#define fscanf_s fscanf
#define sprintf_s sprintf

typedef unsigned int DWORD;
typedef unsigned char BYTE;
typedef char TCHAR, _TCHAR;
typedef int  BOOL;
typedef unsigned long ULONGLONG;
#endif

using std::string;

#define FTDI                0
#define EMMC                1
#define PCIE                2

#define MAX_FILENAME_SIZE   256
#define IMAGE_OUT_SIZE      0x08000
#define GTI_IMAGE_WIDTH     224
#define GTI_IMAGE_HEIGHT    224
#define GTI_IMAGE_CHANNEL   3

#define GTI_PICTURE         0
#define GTI_VIDEO           1
#define GTI_WEBCAM          2

#define PEOPLE_TYPE         7
#define PETS_TYPE           4
#define OFFICE_TYPE         0
#define KITCHEN_TYPE        1
#define LIVINGROOM_TYPE     2


//控制c/c++输出宏
#define OUTINFO_SPEND   1
#define DEBUG_OUT_INFO  0
#define DEBUG_OUT_ERROR 1


//自定义变量宏--------------
#define STATUS_OK       0
#define JLX_GTI_ENDSTATUS_OK  1
#define JLX_GTI_CON_ERROR_CODE 0

//-----
#ifdef WIN32
typedef struct dirent {
    string d_name;
} Dirent, *pDirent;

typedef HANDLE cnn_mutex_t;
#define cnn_mutex_init		WIN_mutex_init
#define cnn_mutex_lock		WIN_mutex_lock
#define cnn_mutex_unlock	WIN_mutex_unlock
#define cnn_mutex_destroy	WIN_mutex_destroy
#endif

#ifdef LINUX
typedef pthread_mutex_t cnn_mutex_t;

#define CNN_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER

#define cnn_mutex_init		pthread_mutex_init
#define cnn_mutex_lock		pthread_mutex_lock
#define cnn_mutex_unlock	pthread_mutex_unlock
#define cnn_mutex_destroy	pthread_mutex_destroy
#endif

typedef enum {
    LOAD_CONFIG_STATUS_FAIL = 10001,//加载配置文件异常
    COEF_FILE_NOT_FOUND = 10002,//Coef File is Not Found,文件未找到
    CREATE_DEVICE_FIAL = 10003, //Jlx Device is null,设备为空异常
    DEVICE_INIT_FIAL = 10004//设备初始化失败
}JLX_SDK_INIT_ERROR_TYPE;

//Allocate memory error type　内存申请错误类型
typedef enum {
    IMAGE_OUTPUT_FAIL = 10005,
    IMAGE_BUFFER_FAIL = 10006,
    IMAGE_INPUT_FAIL  = 10007,
    FILE_NAME_BUFFER_FAIL = 10008
}JLX_ALLOCATE_MEMORY_ERROR_TYPE;

typedef enum{
    ACCESS_CONFIGURE_ERROR = 991,//配置文件权限问题
    OPEN_CONFIGURE_ERROR = 990,//打开配置文件错误
    EMPTY_POINT_ERROR = 989,//空指针设备异常
    NO_SUPPORT_TYPE = 988//不支持该类设备异常
}JLX_CONFIGURE_ERROR_TYPE;


#ifdef ANDROID//add 2018-5-17
typedef struct maxSimilar
{
    int   max_idx;
    float max_similar;
}MaxSimilar;

typedef struct userinfo
{
    char name[64];
    int  index;
}UserInfo;
#endif

namespace JLX {
    namespace DIR {
        int SelfMkdir(const char *nameDir);
        void CheckDirPathExist();
    };
};

//Jsonconfig
namespace JLX{
    namespace JSON{
        class Config{
        private:
            const std::string TAG = "JsonConfigure";
        public:
            Config();
            virtual ~Config();

            void write();
            Json::Reader reader;
            Json::Value  root;
            //Json::FastWriter

            //1.打开
            bool open(const std::string& file);
            //2.解析操作
            void read();

            virtual int parse(int m_GnetType,std::string&   devicenode);
        private:
            ifstream ifs;
        };

        class LoadConfig:public Config{
        public:
            explicit LoadConfig(const std::string& file);
            int parse(int m_GnetType,std::string&   devicenode) override;
            int read_userinput(std::string& configure_file,std::string& device_name);
            float getSimilar();
        private:
            std::string m_prepare_load_file_name;
            const std::string TAG = "LoadConfig";
        public:
            string   similar;
            float    m_usersimilar;
            string   m_coefName;
            string   userConfig;

            string   m_faceFcname;
            string   m_faceLabelname;

            string   m_picFcname;
            string   m_picLabelname;

            string   m_videoFcname;
            string   m_videoLabelname;

            string   m_camFcname;
            string   m_camLabelname;

            string   m_camFc_pets;
            string   m_camLabel_pets;

            string   m_Fc_office;
            string   m_Label_office;

            string   m_Fc_kitchen;
            string   m_Label_kitchen;

            string   m_Fc_livingroom;
            string   m_Label_livingroom;
        };
    };
};


typedef enum {
    WITHOUT_INIT_COMPLETE =101,//在未完成初始化的情况下  去注册  的返回值
    DATA_HAS_PROBLEMS = 102,
    DATA_EMPTY =103,
    INSERT_DATA_ERROR=104
};

class Samples
{
private:
    const std::string TAG = "Samples";
public:
    Samples();
    Samples(JNIEnv* env,jobject object,int);
    Samples(int ,float);
    Samples(const Samples&)= delete;
    Samples& operator=(const Samples&) = delete;
    ~Samples();


    cnn_mutex_t mOpenCvMutex;

    inline int update_userinfo_num(const std::string& name) {
        std::vector<string>::iterator it;
        if(it == userinfo_nums.end()){//没有的话，就压入name
            userinfo_nums.push_back(name);
        }
        return 0;
    }

    int FromVectorCountbyName(std::map<string, int>& vector_count_by_name,
        const  char* name,const char* flag);


    int  SetPackageResourcePath(const char* packageResourcePath);

    int  DelName(const char* name);

    float get_cos_sim(float* test_vector1,
        float* test_vector2,
        int feat_vector_len,
        float abs_of_test_vector1,
        float abs_of_test_vector2);


    int   InitDB(const char* dbPath);

    int   GetSQUserNum();

    void  SetSQUserList(std::vector<std::string>& usernameList);

    int   DeleteFromVector(const char *name);

    template<typename Ta,typename Tb,typename Tc,typename Td>
    void  LoadVectorsFromDB(Ta &filename,
                            Tb &target_vectors,
                            Tc &feat_vector_len,
                            Td &vector_count_by_name);

    int   read_fc7_vector(char* filename, std::vector<float>& vector, int len);

    template <typename Ta,typename Tb,typename Tc>
    static float inner_product(Ta &a,Tb &b,Tc &c){
        float  r = 0.0;
        for (int i = 0; i < c; i++) {
            r += a[i]*b[i];
        }
        return r;
    }

    template <typename Ta,typename Tb>
    static float abs_of_vector(Ta &a,Tb &b) {
        float r = sqrt(inner_product(a, a, b));
        return r;
    }


#ifdef ANDROID
    int     oneFramePredictInit(JNIEnv* env,jobject object,int type);
    int     faceIdentify(unsigned char *imgData1,unsigned char *imgData2);
    int     faceIdentify(const cv::Mat& imgData1,const cv::Mat& imgData2);
    void    oneFramePredict(unsigned char *imgData, string* predictions);
    void    facePredict(const cv::Mat& img,std::vector<std::string>& predictions);
    void    FacePredictC(unsigned char* img,std::vector<std::string>& predictions);
    float*  getFaceFeature(const cv::Mat& img);
    int     GetFeatVectorLen();
    int     face_register(unsigned char *imgData,const char *name);
    int     face_register(const  cv::Mat& imgData,const char *name);
    void    getPredictResult(std::vector<std::string>& predictions);
    int     JlxSDKStep(JNIEnv* env,jobject object);

    int     LoadDB();

template <typename Ta,typename Tb,typename Tc, typename Td>
MaxSimilar get_max_cos_sim_idx(Ta& target_vectors,
        Tb& test_vector,
        Tc& abs_of_target_vectors,
        Td& abs_of_test_vector);


template <typename Ta,typename Tb,typename Tc, typename Td>
MaxSimilar get_TOP5_idx(Ta& target_vectors,
                        Tb& test_vector,
                        Tc& abs_of_target_vectors,
                        Td& abs_of_test_vector);

template <typename target,typename test>
float TwoSimilar(target& ta,test&  te);

float TwoSimilar_C(float* ta,float*  te);

float compare2(unsigned char* img,unsigned char* img2);

#endif


protected:
    int videoFramerate;//视频帧率
    int videoTime;//视频长度 /秒

protected:
    string      pred_name;

    float       m_usersimilar;

    string      m_DBFilePath;

    Mysqlite3   jlxsq;

    bool                    m_initResult;
    JLX::JSON::LoadConfig   m_jsonconfig;

    //add tensorflowlite
#ifdef USE_TFLITE
    JLX::FC::TFModels  m_lite;
#endif

protected:
    std::string m_deviceName;
    string   m_coefName;
    cv::Size m_imgSize;
    // face recognition and prediction
    char  name[128]; // name for register

    int   feat_vector_len; // feature vector dim
    int   num_of_targets; // num of persons in pool//csv 文件数
    std::vector<string> facename_target_vectors;
    std::map<string, int> vector_count_by_name;

    std::vector<float> basevectors;

    std::vector<std::vector<float>> target_vectors;

    std::vector<float> abs_of_target_vectors;

    std::vector<float> out_feat_vector; // feature vector for inference

    std::vector<string> userinfo_nums;
    //end

    GtiDevice   *m_Device = NULL;
    TCHAR       *m_FileNameBuffer = NULL;
    float       *m_Buffer32FC3 = NULL;
    BYTE        *m_Img224x224x3Buffer = NULL;
    float       *m_ImageOutPtr = NULL;
    int         m_WtRdDelay = 35;        // Read/Write delay
    int         m_GnetType;

    ULONGLONG m_SvicTimerStart = 0;
    ULONGLONG m_SvicTimerEnd = 0;
    ULONGLONG m_CnnTimerStart = 0;
    ULONGLONG m_CnnTimerEnd = 0;
    ULONGLONG m_FcTimerStart = 0;
    ULONGLONG m_FcTimerEnd = 0;
    ULONGLONG m_PureCnnStart = 0;
    ULONGLONG m_PureCnnEnd = 0;
    ULONGLONG m_IdentifyTimerStart = 0;
    ULONGLONG m_IdentifyTimerEnd = 0;

#ifdef ANDROID
    Classify *gClassify = NULL;
#endif

#ifdef LINUX
    unsigned long GetTickCount64()
    {
        struct timeval tv;
        if(gettimeofday(&tv, NULL) != 0)
                return 0;
        return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    }
#endif


    void setinitResult(bool result){
        m_initResult = result;
    }

    string openFileDialog(int Mode = 0);
    int    convolution(const cv::Mat& img);
    int    convolution(BYTE* img, const int imgLength);//void  ===>>  int


    void cvt32FloatTo8Byte(float *InBuffer, uchar *OutBuffer, int Width, int Height, int Channels);
    void cnnSvicProc32FC3(const cv::Mat& img, std::vector<cv::Mat>* input_channels);
    void cnnSvicProc8UC3(const cv::Mat& img, std::vector<cv::Mat>*input_channels);



    int  checkfcfilefuntion(const std::string& fcfilepath,const std::string& labelfilepath);
    int  FaceRecognitionInit(Classify *gtiClassify);
    int  face_recognition_register(unsigned char *img_face,const  char* name,Classify* gtiClassify= nullptr);
    int  face_recognition_register(const cv::Mat& img_face,const  char* name,Classify* gtiClassify= nullptr);

#if 0
    int   InitArcSoftDB(const char* dbPath,const std::string& table);
#endif

#ifdef WIN32
    int m_OpenFlag = 0;
    WIN32_FIND_DATA m_ffd;
    Dirent m_ent;
    HANDLE opendir(const char* dirName);
    pDirent readdir(HANDLE);
    void closedir(HANDLE);
#endif
};



