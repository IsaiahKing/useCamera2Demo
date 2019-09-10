#include "Classify.hpp"
#include "Samples.hpp"
#include <unistd.h>
namespace JLX{
    namespace DIR{
        int  SelfMkdir(const char* nameDir){
            if((access(nameDir,F_OK))!=-1){
#if DEBUG_OUT_INFO
                LOG(INFO) << "file exists path:" << nameDir ;
#endif
                return 0;
            }else{
                try {
                    int isCreate = mkdir(nameDir,S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
                    if(!isCreate){
#if DEBUG_OUT_INFO
                        LOG(INFO) << "create path:" << nameDir ;
#endif
                    }
                }
                catch(int create){
#if DEBUG_OUT_ERROR
                    LOG(ERROR) << "create path failed! error code:" << create  << " Dir " << nameDir;
#endif
                }
            }
            return 0;
        }

        void CheckDirPathExist(){
            SelfMkdir("/sdcard/gti/face");
            SelfMkdir("/sdcard/gti/face/Data");
            SelfMkdir("/sdcard/gti/face/Data/vectors/");
            SelfMkdir("/sdcard/gti/face/Data/photos/");
        }

    };
};

namespace JLX{
    namespace MATH{

        float GetFloatRand() {
            union FloatRand r;
            r.BitArea.Signed = 0;
            r.BitArea.Exp = 1;
            r.BitArea.Frac = (rand() * rand()) % 0x800000;

            if (r.BitArea.Frac == 0x7FFFFF)
                r.BitArea.Exp = rand() % 0x7E;
            else if (r.BitArea.Frac == 0)
                r.BitArea.Exp = rand() % 0x7E + 1;
            else
                r.BitArea.Exp = rand() % 0x7F;

            return r.Value;
        }

        float custom_sqrt(float x) {
            if(x == 0) return 0;
            float result = x;
            float xhalf = 0.5f*result;
            int i = *(int*)&result;
            i = 0x5f375a86- (i>>1);
            result = *(float*)&i;
            result = result*(1.5f-xhalf*result*result); // Newton step, repeating increases accuracy
            result = result*(1.5f-xhalf*result*result);
            return 1.0f/result;
        }
    };
};

namespace JLX{
    namespace JSON{
            Config::Config(){};
            Config::~Config(){
                if(ifs.is_open()){
                    ifs.close();
                }
            };

            //void write();
            //1.打开
            bool Config::open(const std::string& file){
                //预打开操作,验证文件权限问题
                int check = access(file.c_str(),R_OK|F_OK);
                if(check == 0){
                    ifs.open(file.c_str());
                    assert(ifs.is_open());
                    return true;
                }
                return false;
            }
            //2.解析操作
            void Config::read(){
                if (!reader.parse(ifs, root, false))
                {
                    cerr << "parse failed \n";
                    return;
                }
            }
            int Config::parse(int m_GnetType,std::string&   devicenode){};

            LoadConfig::LoadConfig(const std::string& file){
                m_prepare_load_file_name = file;
            }

            /***
            * 读取配置configure file 到 devicename
            */
            int LoadConfig::read_userinput(std::string& configure_file,string& device_name){
                int checkfile = access(configure_file.c_str(),R_OK);

                if(checkfile == 0){
                    FILE *file = fopen(configure_file.c_str(), "r");
                    if ( file != nullptr )
                    {
                        char      line[64]={0}; /* or other suitable maximum line size */
                        while ( (fgets(line, sizeof line, file) != nullptr)) /* read a line */
                        {
                            char    *p_index = strstr(line,"node");
                            if(p_index != nullptr){
                                device_name = p_index+6;
                                device_name = device_name.substr(device_name.find("\""),device_name.rfind("\""));
                                device_name = device_name.substr(device_name.find("/"),device_name.rfind("\"")-1);
                                break;
                            }
                        }
                        fclose(file);
                    }else{
                        fclose(file);
                        return OPEN_CONFIGURE_ERROR;
                    }
                }else{
                    return ACCESS_CONFIGURE_ERROR;
                }
                //检测node 是否存在
                //-----
                if(access(device_name.c_str(),R_OK|F_OK) == 0){
#if DEBUG_OUT_INFO
                LOG(INFO) << TAG << " access  "<< " Device Node: " << device_name;
#endif
                }else{
#if DEBUG_OUT_ERROR
                LOG(ERROR) << TAG << " initialization Device Node: " << device_name;
#endif
                }
                return STATUS_OK;
            }

            int LoadConfig::parse(int m_GnetType,std::string&   devicenode){
                open(m_prepare_load_file_name);
                read();
                string sdkpath = root["dataPath"].asString();
                similar = root["similar"].asString();

                string  TFModelFileDir = root["tensorflowlitedir"].asString();
                string  tffile = root["tensorflowlitefile"].asString();

                if(m_GnetType == 0){
                    string        Gnet1Root = root["JlxGnet1Root"].asString();

                    m_coefName  = sdkpath + Gnet1Root +  root["JlxCoefFile"].asString();
                    userConfig  = sdkpath + Gnet1Root +  root["JlxGnet1UserInput"].asString();
#ifdef USE_TFLITE
                    m_faceFcname = sdkpath + TFModelFileDir + tffile;
#else
                    m_faceFcname    = sdkpath + Gnet1Root + root["JlxWebCoefFcFile"].asString();
#endif

                    m_faceLabelname = sdkpath + Gnet1Root + root["JlxWebLabelFileFace"].asString();

                    m_picFcname     = sdkpath + Gnet1Root + root["JlxPicCoefDataFcFileName"].asString();
                    m_picLabelname  =  sdkpath + Gnet1Root + root["JlxPicLabelFileName"].asString();

                    m_videoFcname    = sdkpath + Gnet1Root + root["JlxVideoCoefDataFcFileName"].asString();
                    m_videoLabelname = sdkpath + Gnet1Root + root["JlxVideoLabelFileName"].asString();

                    m_camFcname    = sdkpath + Gnet1Root + root["JlxWebCamCoefDataFcFileName"].asString();
                    m_camLabelname = sdkpath + Gnet1Root + root["JlxWebCamLabelFileName"].asString();

                }else{
                    string Gnet2_3Root = root["JlxGnet2_3Root"].asString();

                    m_coefName = sdkpath + Gnet2_3Root + root["JlxGnet32Coef512"].asString();
                    userConfig = sdkpath + Gnet2_3Root + root["JlxGnet32UserInput"].asString();


                    m_picFcname     = sdkpath + Gnet2_3Root + root["JlxPicCoefDataFcGnet32"].asString();
                    m_picLabelname  =  sdkpath + Gnet2_3Root + root["JlxPicLabelGnet32"].asString();

                    m_camFcname    = sdkpath + Gnet2_3Root + root["JlxWebCamCoefDataFcGnet32"].asString();
                    m_camLabelname = sdkpath + Gnet2_3Root + root["JlxWebCamLabelGnet32"].asString();


                    m_videoFcname = sdkpath    + Gnet2_3Root + root["JlxVideoCoefDataFcGnet32"].asString();
                    m_videoLabelname = sdkpath + Gnet2_3Root + root["JlxVideoLabelGnet32"].asString();
                }
#if DEBUG_OUT_INFO
                LOG(INFO) << TAG << "Read model configure file end";
#endif
                //从配置文件 设置相似值
                std::string& similar_value = similar;
                if(!similar_value.empty()){
                    m_usersimilar = (float)atof(similar_value.c_str());
                    if(m_usersimilar <= 0)
                    {
                        m_usersimilar = 0.75f;
#if DEBUG_OUT_ERROR
                        LOG(ERROR) << "Config similar failed,Default Setting ";
#endif
                    }
                } else{
                    m_usersimilar = 0.75f;
#if DEBUG_OUT_ERROR
                    LOG(ERROR) << "Config similar failed,Because similar is empty, then  Setting Default";
#endif
                }

                return  read_userinput(userConfig,devicenode);
            }

            float LoadConfig::getSimilar(){
                return m_usersimilar;
            }

        };
};


