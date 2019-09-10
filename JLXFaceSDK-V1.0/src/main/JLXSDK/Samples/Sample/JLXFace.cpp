/***************************************************************
 * JLXClassify.cpp
 * JLX2801 output result classify sample code.
 *
 * Copyrights, JLX technology Inc. 2018
 ***************************************************************/
#include "stdafx.h"
#include "Classify.hpp"
#include "Samples.hpp"
#include "Threadpool.hpp"
#include "GtiClassify.h"
#include <math.h>
#include <algorithm>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <android/log.h>
#include <stdint.h>
#include <assert.h>
#include <linux/fs.h>
#include <time.h>
#include <sys/time.h>

#include <jni.h>

#include <scsi/sg.h>
#include <scsi/scsi.h>

#include <linux/mmc/ioctl.h>
#include <sys/capability.h>

#ifdef WIN32
#include "strsafe.h"
#endif
#ifdef LINUX
#include <dirent.h>

#include <logging.h>
#include <fcntl.h>

#endif


using std::string;

//add
#ifdef WIN32
cnn_mutex_t CNN_MUTEX_INITIALIZER()
{
    return CreateMutex(NULL, FALSE, NULL);
}

int WIN_mutex_init(cnn_mutex_t *mutex, void *attr)
{
    *mutex = CreateMutex(NULL, FALSE, NULL);
    return 0;
}

int WIN_mutex_lock(cnn_mutex_t * hMutex)
{
    WaitForSingleObject(*hMutex, INFINITE); /* == WAIT_TIMEOUT; */
    return 0;
}

int WIN_mutex_unlock(cnn_mutex_t * hMutex)
{
    ReleaseMutex(*hMutex);
    return 0;
}

int WIN_mutex_destroy(cnn_mutex_t * hMutex)
{
    CloseHandle(*hMutex);
    return 0;
}
#endif
//end


const static std::string jsonfilepath = "/mnt/sdcard/Jlx/ConfigureFile.json";
string glGtiPath = "GTISDKPATH";

#ifdef WIN32

#else

#ifdef ANDROID
string dataPath = "/sdcard/gti/";
string glImageFolder = dataPath + "Image_bmp";
string glVideoFolder = dataPath + "Image_mp4";
// Model files root
string glGnet1Root = dataPath + "Models/gti2801/gnet1/";
string glGnet2_3Root = dataPath + "Models/gti2801/gnet2_3/";
#else

#endif
// coef file name
string glGtiCoefFileName = glGnet1Root + "cnn/gnet1_coef_vgg16.dat";
string glGnet1UserInput = glGnet1Root + "cnn/userinput.txt";
// Gnet32 coef file name for 512 output
string glGnet32Coef512 = glGnet2_3Root + "cnn_3/gnet32_coef_512.dat";
string glGnet32UserInput = glGnet2_3Root + "cnn_3/userinput.txt";

#endif


int  get_num_of_files(char* directory, string s) {
    int num_of_files = 0;
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(directory)) != NULL){
        /* retrieve all .csv files within directory */
        while((ent = readdir (dir)) != NULL){
            string s1 = ent->d_name;
            string s2 = s;
            if(s1.find(s2) != std::string::npos){
                num_of_files++;
            }
        }
        closedir(dir);
    }else{
#ifdef ANDROID
        LOG(ERROR) << "get_num_of_files Failed to open directory!";
#else
        printf("get_num_of_files Failed to open directory!");
#endif
    }
    return num_of_files;
}

bool GtiResize(unsigned char* imgIn,
               int inHeight,
               int inWidth,
               unsigned char* imgOut,
               int outHeight,
               int outWidth) {
    if(imgIn == NULL) return false;

    double scaleWidth =  (double)outWidth / (double)inWidth;
    double scaleHeight = (double)outHeight / (double)inHeight;

    for(int cy = 0; cy < outHeight; cy++)
    {
        for(int cx = 0; cx < outWidth; cx++)
        {
            int pixel = (cy * (outWidth *3)) + (cx*3);
            int nearestMatch =  (((int)(cy / scaleHeight) * (inWidth *3)) + ((int)(cx / scaleWidth) *3) );

            imgOut[pixel    ] =  imgIn[nearestMatch    ];
            imgOut[pixel + 1] =  imgIn[nearestMatch + 1];
            imgOut[pixel + 2] =  imgIn[nearestMatch + 2];
        }
    }

    return true;
}

bool GtiCvtColor(unsigned char* imgIn,
                 int imgHeight,
                 int imgWidth,
                 unsigned char* imgOut) {
    if(imgIn == NULL) return false;

    for(int cy = 0; cy < imgHeight; cy++)
    {
        for(int cx = 0; cx < imgWidth; cx++)
        {
            int pixel = (cy * (imgWidth *3)) + (cx*3);
            imgOut[cy*imgWidth + cx ] =  imgIn[pixel + 0]; // R
            imgOut[1*imgHeight*imgWidth + cy*imgWidth + cx ] =  imgIn[pixel + 1]; // G
            imgOut[2*imgHeight*imgWidth + cy*imgWidth + cx ] =  imgIn[pixel + 2]; // B
        }
    }

    return true;
}

void get_filenames(char* directory,
                   char** filename,
                   int num_of_files,
                   string s) {
    char dir_vector[256];
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(directory)) != NULL) {
        int i = 0;
        while (((ent = readdir (dir)) != NULL) && (i < num_of_files)) {
            string s1 = ent->d_name;
            string s2 = s;
            if (s1.find(s2) != std::string::npos) {
                strcpy(filename[i], ent->d_name);
                strcpy(dir_vector, directory);
                strcat(dir_vector, ent->d_name);
                strcpy(filename[i], dir_vector);
                i++;
            }
        }
        closedir(dir);
    } else {
        printf("Failed to open directory!");
    }
    return;
}



//---------------------------------------------------------------------------------------------------------------------------

//====================================================================
// Function name: Samples()
// Class Samples construction.
// This function calls JLX library APIs to set device type, open device,
// initialize JLX SDK library. It also allocates memory for the sample
// code.
//====================================================================
Samples::Samples():mOpenCvMutex(CNN_MUTEX_INITIALIZER),m_jsonconfig(jsonfilepath){
    cnn_mutex_init(&mOpenCvMutex, NULL);
}

Samples::Samples(JNIEnv* env,jobject object,int gnetType):
        mOpenCvMutex(CNN_MUTEX_INITIALIZER),
        m_jsonconfig(jsonfilepath),
        m_GnetType(gnetType),
        m_initResult(false),
        videoFramerate(30),
        videoTime(5),
        m_Device(nullptr),
        m_FileNameBuffer(nullptr),
        m_Buffer32FC3(nullptr),
        m_Img224x224x3Buffer(nullptr),
        m_ImageOutPtr(nullptr){

       cnn_mutex_init(&mOpenCvMutex, NULL);


#ifdef LINUX
    #ifdef ANDROID
        //gtiHome = (char*)"";
    #else
        // Get GTISDK home
        gtiHome = getenv(glGtiPath.c_str());
        if (gtiHome == NULL)
        {
            printf("GTISDK home path is not set! Please see GTI SDK user guide.\n");
            exit(0);
        }
    #endif
#endif

}

Samples::Samples(int gnetType,float similar):
        m_GnetType(gnetType),
        m_initResult(false),
        m_jsonconfig(jsonfilepath),
        videoFramerate(30),
        m_usersimilar(similar),
        videoTime(5),
        m_Device(nullptr),
        m_FileNameBuffer(nullptr),
        m_Buffer32FC3(nullptr),
        m_Img224x224x3Buffer(nullptr),
        m_ImageOutPtr(nullptr){

    cnn_mutex_init(&mOpenCvMutex, NULL);
#ifdef LINUX
#ifdef ANDROID
    //gtiHome = (char*)"";
#else
    // Get GTISDK home
        gtiHome = getenv(glGtiPath.c_str());
        if (gtiHome == NULL)
        {
            printf("GTISDK home path is not set! Please see GTI SDK user guide.\n");
            exit(0);
        }
#endif
#endif

}
//====================================================================
// Function name: ~Samples()
// Class Samples destruction.
// This function releases the memory allcated in Samples(), 
// it also calls JLX library API to close device.
//====================================================================
Samples::~Samples() {
    cnn_mutex_destroy(&mOpenCvMutex);
    safeRelease(m_ImageOutPtr);
    safeRelease(m_Img224x224x3Buffer);
    safeRelease(m_Buffer32FC3);
    safeRelease(m_FileNameBuffer);


    // Close device
    if(m_Device != NULL){
        GtiCloseDevice(m_Device);
        // Release device
        GtiDeviceRelease(m_Device);
    }
    if(gClassify != NULL){
        GtiClassifyRelease(gClassify);
    }

}


int Samples::read_fc7_vector(char* filename, std::vector<float>& vector, int len) {
    FILE *file = fopen(filename, "r");
    if ( file != NULL )
    {
        int       i = 0;
        char      line[128]={0}; /* or other suitable maximum line size */
        while ( (fgets(line, sizeof line, file) != NULL) && (i < len)) /* read a line */
        {
            //fputs (line, stdout); /* write the line */
            vector[i] = atof(line);
            i++;
        }
        fclose(file);
    }
    else {
        printf("Failed to open vector file!");
        LOG(ERROR) << "read_fc7_vector Failed to open vector file!";
        return -70;
    }
    return 0;
}

#if 0
int Samples::InitArcSoftDB(const char* dbPath,const std::string& table){
    SelfMkdir(dbPath);
    std::string dbFile(dbPath);
    dbFile = dbFile + "/" + "face.db";
    LOG(INFO) << TAG << " DBFile----> " << dbFile;
    try {
        //1.step sqlite open, db file ...
        int ret1 = jlxsq.open(dbFile.c_str());
        if(!ret1)
        {
#if DEBUG_OUT_INFO
            LOG(INFO) << "Open DB OK";
#endif
        }else{
#if DEBUG_OUT_INFO
            LOG(ERROR) << "Open DB Failed";
#endif
        }
        //2.step should do : sqlite created table..
        const std::string CREATE_TABLE_PRE  ="CREATE TABLE IF NOT EXISTS ";;
        int ret2 = jlxsq.CreateArcTable(table,CREATE_TABLE_PRE);
        if(!ret2)
        {
#if DEBUG_OUT_INFO
            LOG(INFO)<< " Sqlite3_exec  Ok";
#endif
        }

    }
    catch(int ret){
        LOG(ERROR) << TAG << " " << "InitDB " << ret;
    }
    return  0;
}
#endif

//初始化db file
int Samples::InitDB(const char* dbPath){
    JLX::DIR::SelfMkdir(dbPath);
    std::string dbFile(dbPath);
    dbFile = dbFile + "/" + "face.db";
#if DEBUG_OUT_INFO
    LOG(INFO) << TAG << " DBFile PATH :  " << dbFile;
#endif
    try
    {
        //1.step sqlite open, db file ...
        int ret1 = jlxsq.open(dbFile.c_str());
        if(!ret1)
        {
#if DEBUG_OUT_INFO
            LOG(INFO) << "Open DB OK";
#endif
        }else{
#if DEBUG_OUT_INFO
            LOG(ERROR) << "Open DB Failed";
#endif
        }
        //2.step should do : sqlite created table..
        int ret2 = jlxsq.created();
        if(!ret2)
        {
#if DEBUG_OUT_INFO
            LOG(INFO)<< " Sqlite3_exec  Ok";
#endif
        }
    }
    catch(int ret){
        LOG(ERROR) << TAG << " " << "InitDB " << ret;
    }
    return  0;
}



int Samples::FromVectorCountbyName(std::map<string, int>& vector_count_by_name,
                          const  char* name,const char* flag){
    int    count = 0;
    std::map<string, int>::iterator it;
    it = vector_count_by_name.find(std::string(name));

    if (strcmp(flag,"get") == 0){
        if(it != vector_count_by_name.end()){
            count = it->second;
        }else{
            count = 0;//该名字首次注册返回0
        }
    }
    if (strcmp(flag,"update") == 0){
        if(it != vector_count_by_name.end()){
            vector_count_by_name[it->first]++;
        }else{
            vector_count_by_name.insert(std::pair<string, int>(std::string(name), 0));
        }
    }
    return  count;
}


//cnn file network,and user configure file
int Samples::JlxSDKStep(JNIEnv* env,jobject object){
#if OUTINFO_SPEND
    ULONGLONG  m_initTimeStart = GetTickCount64();
#endif

    if(m_jsonconfig.parse(m_GnetType,m_deviceName) != STATUS_OK){
        return LOAD_CONFIG_STATUS_FAIL;
    }

    m_usersimilar = m_jsonconfig.getSimilar();

    //cnn 相关文件指定路径
    //device node name
    // Create  device with eMMC interface
    if((strstr(m_deviceName.c_str(),"sg") != nullptr)|| (strstr(m_deviceName.c_str(),"mmc") != nullptr)){
        LOG(INFO)<< "Samples create  device, "<< (char *)m_jsonconfig.m_coefName.c_str() << " , user: " << (char *)m_jsonconfig.userConfig.c_str();
        if(access(m_jsonconfig.m_coefName.c_str(),F_OK) == 0){
            try{
                m_Device = GtiDeviceCreate(EMMC, (char *)m_jsonconfig.m_coefName.c_str(), (char *)m_jsonconfig.userConfig.c_str());
#if DEBUG_OUT_INFO
                LOG(INFO) << TAG << " Jlx Device Create EMMC ok "<< "use mDevice   = " << m_Device;
#endif
                if(m_Device == nullptr){
#if DEBUG_OUT_ERROR
                    LOG(ERROR) << "m_Device is nullptr  ";
#endif
                }
            }
            catch(...){
                LOG(ERROR) << TAG << " Jlx Device Create Failed";
                throw ;
            }
        }
        else{
#if DEBUG_OUT_ERROR
            LOG(ERROR) << " Coef File is Not Found ";
#endif
            return COEF_FILE_NOT_FOUND;
        }
    }
    else      // Create GTI device with FTDI interface    network_2048.dat   userinput.txt";
    {
        //LOG(INFO) << " Samples Create FTDI device "  << m_jsonconfig.m_coefName  << " , " << m_jsonconfig.userConfig;
        if(access(m_jsonconfig.m_coefName.c_str(),F_OK) == 0){
            try{
                m_Device = GtiDeviceCreate(FTDI, (char *)m_jsonconfig.m_coefName.c_str(), (char *)m_jsonconfig.userConfig.c_str());
#if DEBUG_OUT_ERROR
                LOG(ERROR) << TAG << " Jlx Device Create EMMC ok "<<" use m_Device2 = "<<m_Device;
#endif
            }
            catch(...){
                LOG(ERROR) << TAG << " Jlx Device Create Failed";
                throw ;
            }
            if (m_Device == nullptr){
                LOG(ERROR) << TAG << " Jlx Device is null "<< "m_Device3 = ";
                return CREATE_DEVICE_FIAL;
            }else {
#if DEBUG_OUT_INFO
                LOG(INFO) << TAG << " Jlx Device Create OK";
#endif
            }
        }
        else
        {
#if DEBUG_OUT_ERROR
            LOG(ERROR) << " Coef File is Not Found " ;
#endif
            return COEF_FILE_NOT_FOUND;
        }
    }
#if OUTINFO_SPEND
    ULONGLONG  m_initTimeEnd = GetTickCount64();
    //LOG(INFO) << "Face_JNI: Samples create complete,spend time :"<< (float)(m_initTimeEnd - m_initTimeStart) <<" ms" ;
#endif

#if OUTINFO_SPEND    
    ULONGLONG  m_initSdkTimeStart = GetTickCount64();
#endif

    if (m_Device == nullptr) {
        LOG(ERROR) << "Device is  nullptr" << "m_Device4 = ";
        return CREATE_DEVICE_FIAL;
    }
    //if return value is 1,then ok
    if(m_Device != nullptr)
    {
        try
        {
            if(GtiOpenDevice(m_Device, (char*)m_deviceName.c_str()) == JLX_GTI_ENDSTATUS_OK)
            {
#if DEBUG_OUT_INFO
                LOG(INFO) << TAG <<  " Open Device : " << m_deviceName << " OK ";
#endif
            }
        }catch(std::exception& e)
        {
            LOG(ERROR) << TAG << " Open Device , Failed" << e.what();
        }catch(...)
        {
            LOG(ERROR) << TAG << " Open Device , Failed";
            throw ;
        }

        unsigned int GTI_VERSION = GtiGetSDKVersion(m_Device);
#if DEBUG_OUT_INFO
        LOG(INFO) << TAG << "Version Code: " << GTI_VERSION;
#endif

        if(GtiInitialization(m_Device) == JLX_GTI_ENDSTATUS_OK)// Initialize GTI SDK
        {
#if DEBUG_OUT_INFO
            LOG(INFO) << TAG << " Device initialization OK ";
#endif
        }else
        {
#if DEBUG_OUT_ERROR
            LOG(ERROR) << TAG << "  Device initialization Error code:  ";
#endif
            GtiCloseDevice(m_Device);
            GtiDeviceRelease(m_Device);

            return DEVICE_INIT_FIAL;
        }
    }
    else
    {
#if DEBUG_OUT_ERROR
        LOG(ERROR) << TAG << " Open Device , Failed" << "m_Device5 = ";
#endif
        return CREATE_DEVICE_FIAL;
    }

    //LOG(INFO) << TAG <<" SDKStep Allocate memory for sample code use,begin ";
    // Allocate memory for sample code use
    try {
        m_ImageOutPtr = new float[GtiGetOutputLength(m_Device)];
        LOG(INFO) << "GtiGetOutputLength size :" << GtiGetOutputLength(m_Device);

        if (m_ImageOutPtr == nullptr)
        {
            LOG(ERROR) << "\n\n\n------- GTI allocation (m_ImageOutPtr) failed.";
            return IMAGE_OUTPUT_FAIL;
        }

        m_Buffer32FC3 = new float[GTI_IMAGE_WIDTH * GTI_IMAGE_HEIGHT * GTI_IMAGE_CHANNEL];
        if (!m_Buffer32FC3)
        {
            LOG(ERROR) << "\n\n\n------- GTI allocation (m_Buffer32FC3) failed.";
            return IMAGE_BUFFER_FAIL;
        }

        m_Img224x224x3Buffer = new BYTE[GTI_IMAGE_WIDTH * GTI_IMAGE_HEIGHT * GTI_IMAGE_CHANNEL];
        if (!m_Img224x224x3Buffer)
        {
            LOG(ERROR) <<"\n\n\n------- GTI allocation (m_Img224x224x3Buffer) failed.";
            return IMAGE_INPUT_FAIL;
        }

        m_FileNameBuffer    = new TCHAR[MAX_FILENAME_SIZE];
        if (!m_FileNameBuffer)
        {
            LOG(ERROR) <<"\n\n\n------- GTI allocation (m_FileNameBuffer) failed.";
            return FILE_NAME_BUFFER_FAIL;
        }
    }
    catch(std::bad_alloc& e){
        LOG(ERROR) << TAG << "  " << "GtiSDKStep new space failed  " << e.what();
    }

#if OUTINFO_SPEND
    ULONGLONG  m_initSdkTimeEnd = GetTickCount64();
    auto      sdkspendTime = (float)(m_initSdkTimeEnd - m_initSdkTimeStart);
    LOG(INFO) << " Sdk complete.spend time :" << sdkspendTime;
#endif
    return STATUS_OK;
}
#ifdef ANDROID
//====================================================================
// Function name: oneFramePredictInit()
// This function sets FC and label name for image, video or camera classifer.
//
// Input: none.
// return none.
//====================================================================
int Samples::oneFramePredictInit(JNIEnv* env,jobject object,int type) {
    //fc file 指定路径
#if OUTINFO_SPEND
    ULONGLONG  m_initTimeStart = GetTickCount64();
#endif

#ifdef USE_TFLITE
    long ret = m_lite.init(env,object,m_jsonconfig.m_faceFcname);
    if(ret == STATUS_OK){
        LOG(INFO) << "lite model build ok";
    }else{
        LOG(INFO) << TAG << "lite model build failed";
        exit(-1);
    }
#else
    if(type == 0) //Image
    {
        checkfcfilefuntion(m_jsonconfig.m_picFcname,m_jsonconfig.m_picLabelname);
    }
    else if(type == 1) //Video
    {
        checkfcfilefuntion(m_jsonconfig.m_videoFcname,m_jsonconfig.m_videoLabelname);
    }
    else if(type == 2) //Camera
    {
        checkfcfilefuntion(m_jsonconfig.m_camFcname,m_jsonconfig.m_camLabelname);
    }
    else if(type == 3) //Face
    {
        int get_result = checkfcfilefuntion(m_jsonconfig.m_faceFcname,m_jsonconfig.m_faceLabelname);
        if(STATUS_OK!= get_result){
            LOG(ERROR)<< "GtiClassifyCreate Failed ,Code : " << get_result;
            return get_result;
        }else{
#if OUTINFO_SPEND
            ULONGLONG  m_initTimeEnd = GetTickCount64();
            float      PredictInitspendTime = (float)(m_initTimeEnd - m_initTimeStart);
            LOG(INFO)<< "GtiClassifyCreate: " << PredictInitspendTime ;
#endif
        }

    } else{
#if DEBUG_OUT_ERROR
        LOG(ERROR) << "Samples::oneFramePredictInit type not supported. Type =" << type;
#endif
        return NO_SUPPORT_TYPE;
    }
#endif

    FaceRecognitionInit(gClassify);

    //(image, video, camera,face)
    if(type == 3){
#if DEBUG_OUT_INFO
        LOG(INFO) << TAG << "  cnn_path:"  << m_jsonconfig.m_coefName;
        LOG(INFO) << TAG << "  fc_path : " << m_jsonconfig.m_faceFcname;
#endif
    }
#if DEBUG_OUT_INFO
    LOG(INFO)<< "Samples::oneFramePredictInit. Type = " << type <<" face";
#endif

    setinitResult(true);////gti初始化  (classify)完成标志
    return STATUS_OK;
}
//====================================================================
// Function name: oneFramePredict(unsigned char *imgData, string* predictions)
// This function inputs picture image, sends the image to GTI FPGA,
// gets output from the FPGA, calls Classify functions to get the image
// class name and index.
//
// Input: none.
// return none.
//====================================================================
int Samples::checkfcfilefuntion(const std::string& fcfilepath,const std::string& labelfilepath){
    int result = access(fcfilepath.c_str(),F_OK|R_OK);
    int result_2 = access(labelfilepath.c_str(),F_OK|R_OK);
    if((0 == result)&& (0 == result_2)){
        try{
            gClassify = GtiClassifyCreate(fcfilepath.c_str(), labelfilepath.c_str());
            if(gClassify == nullptr){
                return EMPTY_POINT_ERROR;
            }
        }
        catch(std::exception& e){
            LOG(ERROR) << " Classify Create  thrown "<< e.what();
        }
    }
    else{
        LOG(ERROR) << "check file permission code : " << result << " or : " << result_2;
        LOG(ERROR) << "oneFramePredictInit  File Path"<< fcfilepath.c_str() << " label :" << labelfilepath.c_str();
        return ACCESS_CONFIGURE_ERROR;
    }
    return STATUS_OK;
}

//设置db文件存放路径 /data/usr/0/apK_path/files/db.file
int Samples::SetPackageResourcePath(const char* packageResourcePath){
    const std::string DBFileName = "/files";
    std::string str_path(packageResourcePath);
    str_path    += DBFileName;
    m_DBFilePath = str_path;
#if DEBUG_OUT_INFO
    LOG(INFO) << TAG << " ResourcePath : " << m_DBFilePath;
#endif
    return STATUS_OK;
}

float Samples::get_cos_sim(float* test_vector1,float* test_vector2,
                           int feat_vector_len,
                           float abs_of_test_vector1,
                           float abs_of_test_vector2) {
    float cos_similarity = -1e10;

    cos_similarity  = inner_product(test_vector1, test_vector2, feat_vector_len); //feat_vector_len = 2048
    cos_similarity /= abs_of_test_vector1;
    cos_similarity /= abs_of_test_vector2;

#ifdef ANDROID
    LOG(ERROR) << "--- two pic cos similarity: " << cos_similarity;
#else
    printf("--- two pic cos similarity: %f\n", cos_similarity);
#endif
    return cos_similarity;
}

int Samples::face_register(unsigned char *imgData,const char *name) {
    return face_recognition_register(imgData, name,gClassify);;
}

int Samples::face_register(const cv::Mat& imgData,const char *name) {
    return face_recognition_register(imgData, name,gClassify);
}
#endif

#ifdef FACE_RECOGNITION
int Samples::GetSQUserNum(){//This function: Output UserNum
    int  outNum = 0;
    int  ret    = -1;
    if(!jlxsq.OpenDBStatus)
    {
        InitDB(m_DBFilePath.c_str());
    }

    try {
        ret = jlxsq.GetUserName_FromTable();//操作数据前,确保打开数据库
        if (ret == 0)//操作数据库成功
        {
            outNum = jlxsq.getnameNum();//IF from table get ok,do it;
            if(outNum == 0){
                LOG(ERROR) << TAG << " getSQ user OK,,,";
                return 0;
            }
        }
        else
        {
            LOG(ERROR) << TAG << "getSQ user num error";
            return 0;
        }
    }
    catch(int result){
        LOG(ERROR) << TAG << "  " << result;
    }

    return outNum;
}

void Samples::SetSQUserList(std::vector<std::string>& usernameList){
    try {
        jlxsq.GetNamelist(usernameList);
    }
    catch (std::exception e){
        LOG(ERROR) << TAG << "" << e.what();
    }
}
#endif


int  Samples::face_recognition_register(const cv::Mat& img_face,const char* name,Classify* gtiClassify){
    if(!m_initResult)return WITHOUT_INIT_COMPLETE;

    // CNN
    convolution(img_face);
    // FC

    std::vector<float> feat_vector;
    feat_vector.clear();
#ifdef USE_TFLITE
    m_lite.run(m_ImageOutPtr,feat_vector);
    LOG(INFO) << "face_recognition_register size " << feat_vector.size();
#else
    GtiClassifyFC(gtiClassify, m_ImageOutPtr, 1);
    // get feature vector
    float* v_out = GetFeatureVector(gtiClassify); // gtiClassify->m_Fc7_out
    if(v_out == nullptr)return -102;
    for (int i = 0; i < feat_vector_len; ++i)
    {
        feat_vector.push_back(v_out[i]);
    }
#endif

    static long checkFcOut = std::count(feat_vector.begin(),feat_vector.end(),0);
    //如果算出来　的大部分都是　0 ,fc 有问题...
    if(checkFcOut >= (feat_vector_len/2)){
        LOG(ERROR) <<"face_recognition_register data has problems";
        return  DATA_HAS_PROBLEMS;
    }
    if(feat_vector.empty()){
        return  DATA_EMPTY;
    }

    // insert new feature vector
    target_vectors.emplace_back(feat_vector);

    // insert abs of new feature vector
    abs_of_target_vectors.emplace_back(abs_of_vector(feat_vector, feat_vector_len)); // calculate the abs of the newly added vector

    // update number of target feature vectors
    num_of_targets++;

    //update vector count of each person
    FromVectorCountbyName(vector_count_by_name, name,"update");

    std::string nameI = name;

    update_userinfo_num(nameI);

    // get feature vector filename
    char*  fn_vector = new char[64];
    memset(fn_vector,0,64);

    int   index = FromVectorCountbyName(vector_count_by_name, name,"get");
    sprintf(fn_vector, "%s_%04d", name, index);

    delete[] fn_vector;
    fn_vector = nullptr;

    //LOG(INFO) << "insertToDB  "<< " name: "<< fn_vector;

    // insert file name of new feature vector
    facename_target_vectors.emplace_back(nameI);//在数据库 的名字

    if(STATUS_OK != jlxsq.InsertToDB(feat_vector,nameI,index)){
        return INSERT_DATA_ERROR;
    }

    return  STATUS_OK;
}

int  Samples::face_recognition_register(unsigned char *img_face,const char* name,Classify* gtiClassify){
    if(!m_initResult){
        return 20001;//在未完成初始化的情况下去注册返回值
    }

    int  Ret = 0;

    cv::Mat bgr(224,224,CV_8UC3,(unsigned char*)img_face);
    static int icountimage=0;
    char fname[50] = {0};
    sprintf(fname,"/sdcard/jlx/%d.jpg",icountimage++);
    IplImage ipm = IplImage(bgr);
    cvSaveImage(fname,&ipm,0);


    // CNN
    ULONGLONG  CnnTimerStart = GetTickCount64();

    int conRet = convolution(img_face, GTI_IMAGE_WIDTH * GTI_IMAGE_HEIGHT * 3);

    ULONGLONG  CnnTimerEnd = GetTickCount64();

    if(conRet != 0)
    {
        //-95, convolution error
        LOG(ERROR) << "Face Register  "<< "Error Code:"<< conRet << " Because: convolution error ";
        return conRet;
    }
    // FC
    ULONGLONG Fc_TimerStart = GetTickCount64();
    GtiClassifyFC(gtiClassify, m_ImageOutPtr, 1);
    ULONGLONG Fc_TimerEnd = GetTickCount64();
    LOG(INFO) << " FaceRegister  GtiClassifyFC" << " end:"<< Ret;

    float* v_out = GetFeatureVector(gtiClassify); // gtiClassify->m_Fc7_out

    std::vector<float>  feat_vector;
    for (int i = 0; i < feat_vector_len; ++i)
    {
        feat_vector[i] = v_out[i];
    }
    // insert new feature vector
    target_vectors.push_back(feat_vector);

    // insert abs of new feature vector 
    abs_of_target_vectors.push_back(abs_of_vector(feat_vector, feat_vector_len)); // calculate the abs of the newly added vector

    // update number of target feature vectors
    num_of_targets++;

    //update vector count of each person 
    FromVectorCountbyName(vector_count_by_name, name,"update");

    update_userinfo_num(string(name));

    //LOG(INFO) << "user info num name: "<< name;

    // get feature vector filename 
    char  fn_vector[128] = {0};

    int   index = FromVectorCountbyName(vector_count_by_name, name,"get");
    sprintf(fn_vector, "%s_%04d", name, index);

    LOG(INFO) << "insertToDB  "<< "name:"<< fn_vector;

    // insert file name of new feature vector 
    facename_target_vectors.push_back(string(name));

    string insert_name = string(name);
    jlxsq.InsertToDB(feat_vector,insert_name,index);

    return Ret;
}

int  Samples::DeleteFromVector(const char *name) {
   if(strcmp(name,"deleteall") == 0)
   {//清除全部
       if(facename_target_vectors.size() >= 0)
       {
           facename_target_vectors.clear();
           abs_of_target_vectors.clear();
           target_vectors.clear();
           return 0;
       }
   }

   std::vector<int>   data;
   int   ret = jlxsq.GetUserName_FromTable();
   if(ret == 0) {
       jlxsq.comp_vector(data,name);
       LOG(ERROR) << "delete step = 1,get table name num : "<< data.size() << "name: " << name << " ##";
   }
   else {
       return -30;
   }

   //擦除
//   facename_target_vectors.erase(
//            std::remove(std::begin(facename_target_vectors),
//                        std::end(facename_target_vectors),std::string(name)),
//            std::end(facename_target_vectors));

    std::vector<string>::iterator                iter    = facename_target_vectors.begin();
    std::vector<float>::iterator                 iter_1  = abs_of_target_vectors.begin();
    std::vector<std::vector<float>>::iterator    iter_2  = target_vectors.begin();

   for(iter;iter != facename_target_vectors.end();){
       if(strcmp((*iter).c_str(),name) == 0){
           facename_target_vectors.erase(iter);
           abs_of_target_vectors.erase(iter_1);
           target_vectors.erase(iter_2);
           num_of_targets--;
       }else{
           iter++;
           iter_1++;
           iter_2++;
       }
       if(iter == facename_target_vectors.end()){
           break;
       }
   }

   return 0;
}

int  Samples::DelName(const char* name){
    DeleteFromVector(name);//操作（删除数据结构中的内容）
    return jlxsq.DeleteFromTableWhereName(name);
}

std::string Samples::openFileDialog(int Mode) {
#ifdef LINUX
    TCHAR fileName[200];
    std::string retFileName = "Image_bmp/bicycle.bmp";

    //std::cout << "************************" << std::endl;
    switch (Mode)
    {
    case GTI_PICTURE:    // picture only
        //std::cout << "Please input image file name(*.bmp, *.jpg, *.png): ";
        scanf_s("%s", fileName);
        getchar();
        retFileName = (std::string)&fileName[0];
        break;
    case GTI_VIDEO:      // video only
        //std::cout << "Please input video file name(*.mp4): ";
        scanf_s("%s", fileName);
        getchar();
        retFileName = (std::string)&fileName[0];
        break;
    default:             // unknown
        //std::cout << "Unknown file mode, open image file - Image_bmp/bicycle.bmp" << std::endl;
        break;
    }
    //std::cout << "************************" << std::endl;
    return retFileName;
#else   
    // Initialize OPENFILENAME
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = m_FileNameBuffer;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_FILENAME_SIZE;
    switch (Mode)
    {
    case GTI_PICTURE:     // picture only
        ofn.lpstrInitialDir = (LPCWSTR)glImageFolder.c_str();// L".\\Image_bmp\\";
        ofn.lpstrFilter = L"Image Files(*.BMP;*.JPG;*.PNG;)\0*.BMP;*.JPG;*.PNG;\0All files(*.*)\0*.*\0";
        break;
    case GTI_VIDEO:       // video only
        ofn.lpstrInitialDir = (LPCWSTR)glVideoFolder.c_str();  // L".\\Image_mp4\\";
        ofn.lpstrFilter = L"Video Files(*.mp4)\0*.mp4;\0All files(*.*)\0*.*\0";
        break;
    default:    // picture and video
        ofn.lpstrInitialDir = (LPCWSTR)glImageFolder.c_str(); // L".\\Image_bmp\\";
        ofn.lpstrFilter = L"Image Files(*.BMP;*.JPG;*.PNG;*.mp4)\0*.BMP;*.JPG;*.PNG;*.mp4;\0All files(*.*)\0*.*\0";
        break;
    }

    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = L"Open Image..."; // NULL;
    ofn.nMaxFileTitle = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    // Display the Open dialog box. 
    if (GetOpenFileName(&ofn) == TRUE)
    {
        std::wstring ws(ofn.lpstrFile);                 // wchar_t to wstring
        return std::string(ws.begin(), ws.end());       // wstring to string
    }
    else
    {
        return std::string("");
    }
#endif
}

#ifdef WIN32
HANDLE Samples::opendir(const char* dirName)
{
    HANDLE hDir = INVALID_HANDLE_VALUE;
    wchar_t szDir[MAX_PATH];
    size_t ret;

    mbstowcs_s(&ret, szDir, MAX_PATH, dirName, MAX_PATH);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
    hDir = FindFirstFile(szDir, &m_ffd);
    if (INVALID_HANDLE_VALUE == hDir)
    {
        hDir = NULL;
    }
    m_OpenFlag = 1;
    return hDir;
}

pDirent Samples::readdir(HANDLE hDir)
{
    char fileName[200];
    int fsize;

    if (0 == m_OpenFlag)
    {
        if (0 == FindNextFile(hDir, &m_ffd))
        {
            return NULL;
        }
    }
    m_OpenFlag = 0;
    fsize = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, m_ffd.cFileName, -1, fileName, 0, NULL, NULL);
    fsize = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, m_ffd.cFileName, -1, fileName, fsize, NULL, NULL);
    m_ent.d_name = fileName;
    return &m_ent;
}

void Samples::closedir(HANDLE hDir)
{
    FindClose(hDir);
}
#endif

//end
