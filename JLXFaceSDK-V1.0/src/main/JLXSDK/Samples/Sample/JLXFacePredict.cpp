#include "Classify.hpp"
#include "Samples.hpp"
#include "Threadpool.hpp"
#include "GtiClassify.h"
#include <math.h>
#include <unistd.h>


#if ACCUMLATED_TEST
float frameRate = 0.0;
float avgCnnProcessTime = 0.;
float accumlatedCnnProcessTime = 0.;
float avgFcProcessTime = 0.;
float accumlatedFcProcessTime = 0.;
int   frame_num = 0;
float accumlatedRecognitionProcessTime = 0.;
float avgRecognitionProcessTime = 0.;
float recognitionProcessTime = 0.;
#endif
float cnnProcessTime_ms = 0;
float fcProcessTime_ms = 0;

int Samples::LoadDB(){

    //打开数据库操作
    if(!jlxsq.OpenDBStatus){
        InitDB(m_DBFilePath.c_str());
    }

    //当前数据库中的人名数，用于初始化容器
    num_of_targets  = jlxsq.GetAllRowFromTable();

#ifdef ANDROID
#if DEBUG_OUT_INFO
    LOG(INFO) << TAG <<"  Face NumOf Face = " << num_of_targets << " feat_vector_length = " << feat_vector_len;
#endif
#else
    printf("Face num_of_targets = %d, feat_vector_len = %d\n", num_of_targets, feat_vector_len);
#endif
    //没数据,或是得到的数据为空返回
    if(num_of_targets == 0){
        LOG(ERROR) << TAG << " Face Num Face = 0 ,so exit 1";
        return  STATUS_OK;
    }
    try {
        std::vector<float> v_ptr_vector;
        for(int i = 0;i < feat_vector_len;++i){
            v_ptr_vector.push_back(0);
        }

        for(int i = 0; i < num_of_targets; i++){
            facename_target_vectors.emplace_back(string(""));
            // allocate memory for target vectors
            target_vectors.emplace_back(v_ptr_vector);
            // allocate memory for abs of target vectors
            abs_of_target_vectors.push_back(0.0);
        }
    }
    catch(std::exception& e){
        LOG(ERROR) << TAG << "  " << "FaceRecognitionInit "<< e.what();
    }
    //load target vectors //加载csv数据到target_vectors(容器)
#if DEBUG_OUT_INFO
    LOG(INFO) << TAG << "Load Vectors  FromDB  begin";
#endif
    LoadVectorsFromDB(facename_target_vectors, target_vectors, feat_vector_len, vector_count_by_name);
    //特征绝对值
    // pre-calculate abs of target vectors
    for(int i = 0; i < num_of_targets; i++)
    {
        abs_of_target_vectors.at(i) = abs_of_vector(target_vectors.at(i), feat_vector_len);
    }

#if DEBUG_OUT_INFO
    LOG(INFO) << TAG << "Load Vectors  FromDB  end ";
#endif
    return STATUS_OK;
}

int Samples::FaceRecognitionInit(Classify* gtiClassify) {
#if OUTINFO_SPEND
    ULONGLONG  m_initTimeStart = GetTickCount64();
#endif
    // set directory of target vectors and photos
    //检查目录是否存在
    JLX::DIR::CheckDirPathExist();

    //获取特征长度
    //get feature vector length
    if(gtiClassify != nullptr){
        feat_vector_len = GetFeatureVectorLength(gtiClassify);
    }else{
#ifdef USE_TFLITE
        feat_vector_len = m_lite.GetOutPutSize();
#endif
    }
    LOG(ERROR) << "get feat_vector_len:  " << feat_vector_len << " completed ...";
    return LoadDB();
}

float* Samples::getFaceFeature(const cv::Mat& img){
    //没有初始化完成,返回
    if(!m_initResult){
#if DEBUG_OUT_ERROR
        LOG(ERROR)  << "init not OK,please wait";
#endif
        return nullptr;
    }

    if(STATUS_OK != convolution(img)){
#if DEBUG_OUT_ERROR
        LOG(ERROR) << "convolution error ,please again";
#endif
        return nullptr;
    }
    gClassify->GtiClassify(m_ImageOutPtr, 1);//7*7*512
    return GetFeatureVector(gClassify);
}

//img  224*224 size
float Samples::compare2(unsigned char* img,unsigned char* img2){
    //没有初始化完成,返回
    if(!m_initResult){
#if DEBUG_OUT_ERROR
        LOG(ERROR)  << "init not OK,please wait";
#endif
        return 0;
    }
    if(STATUS_OK != convolution(img,GTI_IMAGE_WIDTH * GTI_IMAGE_HEIGHT * 3)){
#if DEBUG_OUT_ERROR
        LOG(ERROR) << "convolution error ,please again";
#endif
        return 0;
    }else{
#ifdef LINUX_C_DEBUG
        printf(" %s convolution ok %s\n",font.green,font.end);
#endif
    }

    GtiClassifyFC(gClassify, m_ImageOutPtr, 1);
    float* fc7_out = GetFeatureVector(gClassify);

    std::vector<float>  test_vector_feat_vector;

    memcpy(test_vector_feat_vector.data(),fc7_out,feat_vector_len*sizeof(float));

    //....
    if(STATUS_OK != convolution(img2,GTI_IMAGE_WIDTH * GTI_IMAGE_HEIGHT * 3)){
#if DEBUG_OUT_ERROR
        LOG(ERROR) << "convolution error ,please again";
#endif
        return 0;
    }else{
#ifdef LINUX_C_DEBUG
        printf(" %s convolution ok %s\n",font.green,font.end);
#endif
    }

    GtiClassifyFC(gClassify, m_ImageOutPtr, 1);
    float* fc7_out2 = GetFeatureVector(gClassify);

    std::vector<float> test_vector_feat_vector2;

    memcpy(test_vector_feat_vector2.data(),fc7_out2,feat_vector_len*sizeof(float));

    return TwoSimilar(test_vector_feat_vector,test_vector_feat_vector2);
}

int Samples::GetFeatVectorLen(){
    return feat_vector_len;
}

void Samples::getPredictResult(std::vector<std::string>& predictions){

#ifdef USE_TFLITE
    if(out_feat_vector.empty())return;
#else
    //得到特征向量
    predictions.clear();
    if(gClassify == nullptr)
        return;
    float* fc7_out = GetFeatureVector(gClassify);
    if(fc7_out == nullptr){
        return;
    }
//    else{
//        LOG(INFO) << "GetFeatureVector--- end----clear ----";
//        return;
//    }
    for (int i = 0; i < feat_vector_len; ++i) {
        out_feat_vector.push_back(fc7_out[i]);
    }
    //输入人脸图像 注释1
    //memset(out_feat_vector.data(),0,feat_vector_len*sizeof(float));
    //memcpy(out_feat_vector.data(),fc7_out,feat_vector_len*sizeof(float));
#endif

#if DEBUG_OUT_INFO
    LOG(INFO) << "GetFeatureVector--- end";
#endif
    const static int num_basic_targets = 2000;
    if(num_of_targets > 0){
        //自定义结构
        MaxSimilar  m_maxIdx={0};
        float       abs_of_feat_vector = abs_of_vector(out_feat_vector, feat_vector_len);//绝对值
        if (num_of_targets < num_basic_targets){
            //ULONGLONG   t1Time = GetTickCount64();
            m_maxIdx    = get_max_cos_sim_idx(target_vectors, out_feat_vector ,abs_of_target_vectors, abs_of_feat_vector);
            //m_maxIdx    = get_TOP5_idx(target_vectors, feat_vector ,abs_of_target_vectors, abs_of_feat_vector);
            //float loopCompa = (float)(GetTickCount64() - t1Time);
            //LOG(INFO)<< " one thread get_max_cos ----   : "<< loopCompa;
        }
        else{

        }
        LOG(INFO) << "similar : " <<  m_maxIdx.max_similar;
        if(m_maxIdx.max_idx == -1){
            //访客...
            //LOG(INFO)<< "Max_idx  is -1";
            return ;
        }
        else{
            if (!facename_target_vectors.empty()){
                if(m_maxIdx.max_idx >= 0){
                    pred_name = facename_target_vectors.at(m_maxIdx.max_idx);
                    predictions.emplace_back(pred_name);
                }
            }
        }
    }
    //add  18-05-17
#if 0
    accumlatedRecognitionProcessTime +=  (GetTickCount64() - m_FcTimerEnd);
    // calculate frame rate every 10 frames
    if (frame_num % FRAME_COUNT == 0)
    {

        m_SvicTimerEnd = GetTickCount64();
        avgCnnProcessTime = accumlatedCnnProcessTime / FRAME_COUNT;
        accumlatedCnnProcessTime = 0.;
        avgFcProcessTime = accumlatedFcProcessTime / FRAME_COUNT;
        accumlatedFcProcessTime = 0.;
        avgRecognitionProcessTime = accumlatedRecognitionProcessTime / FRAME_COUNT;
        accumlatedRecognitionProcessTime = 0.;
        frameRate = (1000 / (avgCnnProcessTime + avgFcProcessTime + avgRecognitionProcessTime));
    //  sprintf_s(charbuf, "Frame Rate: %5.2f, Process(ms) - CNN: %5.2f ms, FC: %5.2f ms, Recognition: %5.2f ms",
    //            frameRate, avgCnnProcessTime, avgFcProcessTime, avgRecognitionProcessTime);
    //  ALOG("--- %s", charbuf);
    }
#endif
}

void Samples::facePredict(const cv::Mat& img,std::vector<std::string>& predictions){
    predictions.clear();
    if(!m_initResult)return;
    if(img.empty())return;

    out_feat_vector.clear();//存放输出feature
    m_CnnTimerStart = GetTickCount64();
    convolution(img);
    m_CnnTimerEnd = GetTickCount64();

    m_FcTimerStart = m_CnnTimerEnd;
#ifdef USE_TFLITE
    m_lite.run(m_ImageOutPtr,out_feat_vector);
    #if OUTINFO_SPEND
        m_FcTimerEnd = GetTickCount64();
        cnnProcessTime_ms = static_cast<float>(m_CnnTimerEnd-m_CnnTimerStart);
        fcProcessTime_ms = static_cast<float>(m_FcTimerEnd - m_FcTimerStart);
        LOG(INFO)  << " cnn = " << cnnProcessTime_ms << " ms" << " fc=　" << fcProcessTime_ms << "ms";
    #endif
#else
    if(gClassify != NULL){
        gClassify->GtiClassify(m_ImageOutPtr, 1);
        LOG(INFO) << TAG << " ---gClassify GtiClassify facePredict completed --- ";
    }
#endif

    getPredictResult(predictions);

#if DEBUG_OUT_INFO
    if(predictions.empty()){
        LOG(INFO) << TAG << " ---facePredict completed --- ";
    }else{
        LOG(INFO) << TAG << " ---facePredict completed --- ok ---";
    }

#endif
}

void Samples::FacePredictC(unsigned char* img,std::vector<std::string>& predictions) {
    //首先清空识别结果集,得到识别后的记录保存在这数据..
    predictions.clear();
    //没有初始化完成,返回
    if(!m_initResult)return;
    if(img == nullptr)return;
    //帧大小

    //static const  int FRAME_COUNT = 20;
    m_CnnTimerStart = GetTickCount64();
    //如果卷积 不ok ,返回

    if(STATUS_OK != convolution(img, GTI_IMAGE_WIDTH * GTI_IMAGE_HEIGHT * 3)){
        return;
    }

    m_CnnTimerEnd  = GetTickCount64();
    m_FcTimerStart = m_CnnTimerEnd;

    gClassify->GtiClassify(m_ImageOutPtr, 1);

    m_FcTimerEnd = GetTickCount64();
    cnnProcessTime_ms = (float)(m_CnnTimerEnd - m_CnnTimerStart);
    fcProcessTime_ms  = (float)(m_FcTimerEnd - m_FcTimerStart);

#if ACCUMLATED_TEST
    accumlatedCnnProcessTime += cnnProcessTime_ms;
    accumlatedFcProcessTime += fcProcessTime_ms;
#endif

#if DEBUG_OUT_INFO
    char cnn_process_time[16]={0};
    sprintf(cnn_process_time,"%2.0f",cnnProcessTime_ms);
    char fc_process_time[16]={0};
    sprintf(fc_process_time,"%2.0f",fcProcessTime_ms);
    LOG(INFO) << "CNN= "<< cnn_process_time << "ms " <<" FC=　" << fc_process_time << "ms";
#endif

    getPredictResult(predictions);
}

void Samples::oneFramePredict(unsigned char *imgData, string* predictions){
    const static int FRAME_COUNT = 30;
    m_CnnTimerStart = GetTickCount64();

    convolution(imgData, GTI_IMAGE_WIDTH * GTI_IMAGE_HEIGHT * 3);

    m_CnnTimerEnd = GetTickCount64();
    m_FcTimerStart = m_CnnTimerEnd;

    gClassify->GtiClassify(m_ImageOutPtr, 3);

    m_FcTimerEnd = GetTickCount64();

    cnnProcessTime_ms = (float)(m_CnnTimerEnd - m_CnnTimerStart);
    fcProcessTime_ms  = (float)(m_FcTimerEnd - m_FcTimerStart);
#if ACCUMLATED_TEST
    accumlatedCnnProcessTime += cnnProcessTime_ms;
    accumlatedFcProcessTime  += fcProcessTime_ms;

    frame_num++;
    // calculate frame rate every 10 frames
    if (frame_num % FRAME_COUNT == 0) 
    {
        m_SvicTimerEnd = GetTickCount64();

        avgCnnProcessTime = accumlatedCnnProcessTime / FRAME_COUNT;
        accumlatedCnnProcessTime = 0.;
        avgFcProcessTime = accumlatedFcProcessTime / FRAME_COUNT;
        accumlatedFcProcessTime = 0.;
        frameRate = (1000 / (avgCnnProcessTime + avgFcProcessTime));

    }
    /* Print the top N predictions. */
    //for (int i = 0; i < gClassify->GtiGetPredicationSize(); ++i)
    for (int i = 0; i < 2; ++i)
    {
        //  char *ptext = gClassify->GtiGetPredicationString(i);
        //  ALOG("--- ENTER Samples::oneFramePredict prediction[%d].\n", i);

        //std::cout << std::fixed << std::setprecision(4) << ptext << std::endl;
        predictions[i] = gClassify->GtiGetPredicationString(i);
        //ALOG("--- Prediction: [%s] \n", predictions[i].c_str());

        //std::to_string(m_Predications[Index].second) + string(" - ") + m_Predications[Index].first;
    }
#endif
}


template<typename Ta,typename Tb,typename Tc,typename Td>
void  Samples::LoadVectorsFromDB(Ta &filename,
                                 Tb &target_vectors,
                                 Tc &feat_vector_len,
                                 Td &vector_count_by_name){

    int ret = jlxsq.prepare();
    if(!ret){
#if DEBUG_OUT_INFO
    LOG(INFO) << " sqlite3_exec  prepare ";
#endif
        jlxsq.ReadDBtoText(filename, feat_vector_len, target_vectors);
    }

    if((!filename.empty()))
    {
        if (vector_count_by_name.empty())
        {
            string  s1 = filename[0];
#if DEBUG_OUT_INFO
    LOG(INFO)<< "IF count empty:  " << s1.c_str();
#endif
            if (s1.length() > 0)
            {
                //s1.erase(s1.length()-4,4);
                vector_count_by_name.insert(std::pair<string, int>(s1, 0));
#if DEBUG_OUT_INFO
    LOG(INFO) << "insert name ---------"<< s1.c_str();
#endif
            }
        }
        else
        {
            for (int i = 0; i < jlxsq.db_feat_name.size(); ++i)
            {
                auto it = vector_count_by_name.find(jlxsq.db_feat_name[i]);
                if (it != vector_count_by_name.end())
                {
                    vector_count_by_name[it->first]++;
                }
                else
                {
                    string  s1 = filename[i];
                    //s1.erase(s1.length()-4,4);
                    vector_count_by_name.insert(std::pair<string, int>(s1, 0));//s1 is string,name
                    //userinfo_nums.push_back(s1); //add
                }
#if DEBUG_OUT_INFO
    LOG(INFO)<< "vector_count_by_name.insert";
#endif
            }
        }
    }
    #if DEBUG_OUT_INFO
    LOG(INFO) << "load_vectors_from_DB  completed";
    #endif
}


static bool myComp(MaxSimilar A,MaxSimilar B){
    return  A.max_similar < B.max_similar;
}


template <typename Ta,typename Tb,typename Tc, typename Td>//a thread
MaxSimilar Samples::get_TOP5_idx(Ta& target_vectors,
                                        Tb& test_vector,
                                        Tc& abs_of_target_vectors,
                                        Td& abs_of_test_vector) {

    MaxSimilar   out;
    out.max_idx = -1;
    out.max_similar = 0;

    if(m_usersimilar <= 0) {
        return out;
    }

    float   max_sim = -1e10;
    float   s = 0;

    const int  local_num_of_target = target_vectors.size();

    std::vector<MaxSimilar > m_top5V_total;
    m_top5V_total.clear();

    for(int i = 0; i < local_num_of_target; i++) {
        s = inner_product(target_vectors[i], test_vector, feat_vector_len);//s  found max
        LOG(INFO) << "similar  abs_of_target :" << abs_of_target_vectors[i] <<"  "<< abs_of_test_vector ;
        s /= abs_of_target_vectors[i];
        s /= abs_of_test_vector;



        MaxSimilar value;
        value.max_similar = s;
        value.max_idx = i;

        m_top5V_total.push_back(value);
#if DEBUG_OUT_INFO
        LOG(INFO) << "out of ---- similar " << s;
#endif
    }

    if(m_top5V_total.empty()){
        return out;
    }

    //默认按升序排序
    std::sort(m_top5V_total.begin(),m_top5V_total.end(),myComp);

    size_t total_length = m_top5V_total.size();

    const int top_index = 5;//取最前面的x位

    const static int registerNumber = 5;//注册人数

    std::vector<std::string> v_candidate;//候选人
    string  findName;

    const size_t begin_index = total_length - top_index;

    if(total_length > top_index){
        for (int i = 0; i < top_index; ++i) {
            // 取对应的id
            int& value = m_top5V_total.at(begin_index + i).max_idx;

            v_candidate.push_back(facename_target_vectors.at(value));//存入
#if DEBUG_OUT_INFO
            LOG(INFO)  << "out of topV5  " << " index: " << i << "  similar "<< m_top5V_total.at(begin_index + i).max_similar;
#endif
        }
    }

    if(!v_candidate.empty()){
        long votes = 1;

        for(int i = 0;i < v_candidate.size();i++){
            //列举每一个元素出现的次数
            long   votes_tmps = std::count(v_candidate.begin(),v_candidate.end(),v_candidate.at(i));

            if(votes_tmps > votes){
                votes = votes_tmps;
                findName = v_candidate.at(i); //name
            }
        }
        // name -> id

#if DEBUG_OUT_INFO
        LOG(INFO) << "out of max votes " << votes;
#endif
        if(votes > 1){
            int     ok = 0;
            float   s  = 0;
            int    find_top_max_idx = 0;

            for(int i = 0;i < v_candidate.size();i++){
                float& m = m_top5V_total.at(begin_index + i).max_similar;
                if(m > m_usersimilar){
                    ok++;
                    if(m > s){
                        s = m;
                        find_top_max_idx = i;
                    }
                }
            }
            if(ok >= registerNumber*0.6){
                out = m_top5V_total.at(begin_index + find_top_max_idx);
            }
        }else{//每个 击中一次 ,则取最大概率
            MaxSimilar& value = m_top5V_total.at(total_length - 1);
            float& out_value_similar = value.max_similar;
            if(out_value_similar >= m_usersimilar){
                out = value;
            }
        }
#if DEBUG_OUT_INFO
        LOG(INFO) << "out of topV5 max  " << "  index " << out.max_idx << " similar " << out.max_similar;
#endif
    }

    return out;
}



template <typename target,typename test>
float Samples::TwoSimilar(target& ta,test&  te){
    float s;
    s = inner_product(ta, te, feat_vector_len);//s  found max
    s /= abs_of_vector(ta,feat_vector_len);
    s /= abs_of_vector(te,feat_vector_len);
    return s;
};

float Samples::TwoSimilar_C(float* ta,float*te){
    float s;
    s = inner_product(ta, te, feat_vector_len);//s  found max
    s /= abs_of_vector(ta,feat_vector_len);
    s /= abs_of_vector(te,feat_vector_len);
    return s;
};

template <typename Ta,typename Tb,typename Tc, typename Td>//a thread
MaxSimilar Samples::get_max_cos_sim_idx(Ta& target_vectors,
                                        Tb& test_vector,
                                        Tc& abs_of_target_vectors,
                                        Td& abs_of_test_vector) {

    MaxSimilar   out ={0};
    if(m_usersimilar <= 0) {
        out.max_idx = -1;
        out.max_similar = 0;
        return out;
    }

    int     max_idx = -1;
    float   max_sim = -1e10;
    float   s = 0;

    const int  local_num_of_target = target_vectors.size();

    for(int i = 0; i < local_num_of_target; i++) {
        s = inner_product(target_vectors[i], test_vector, feat_vector_len);//s  found max
        //LOG(INFO) << "similar  abs_of_target :" << abs_of_target_vectors[i] <<"  "<< abs_of_test_vector ;
        s /= abs_of_target_vectors[i];//s found abs_target min
        s /= abs_of_test_vector;

#if DEBUG_OUT_INFO
        //LOG(INFO) << "get cos similarity:" << s;
#endif
        if(s > max_sim) {
            max_sim = s;
            max_idx = i;
        }
    }

    out.max_similar = max_sim;
    if(max_sim < m_usersimilar){
        out.max_idx   = -1;
#if DEBUG_OUT_INFO
        LOG(INFO) << " similarity:" << max_sim << " id:" << max_idx;
#endif
    }else{
        out.max_idx = max_idx;
    }

    return out;
}