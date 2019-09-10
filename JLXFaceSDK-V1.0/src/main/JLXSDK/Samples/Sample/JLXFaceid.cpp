#include "Classify.hpp"
#include "Samples.hpp"
#include "Threadpool.hpp"
#include "GtiClassify.h"

float faceIdentifyProcessTime = 0.;

int Samples::faceIdentify(unsigned char *imgData1,unsigned char *imgData2){
    int Ret = 0;
    char  charbuf[256];
    m_IdentifyTimerStart = GetTickCount64();

    // handle imgData1
    int cRet = convolution(imgData1, GTI_IMAGE_WIDTH * GTI_IMAGE_HEIGHT * 3);
    if(cRet != 0)
    {
        Ret = -103;// 103 ,convolution error
    }

    // FC
    GtiClassifyFC(gClassify, m_ImageOutPtr, 1);

    // get feature vector
    float* v_out1 = GetFeatureVector(gClassify); // gClassify->m_Fc7_out
    float* feat_vector1 = new float[feat_vector_len]; // feature vector
    for (int i = 0; i < feat_vector_len; ++i) {
        feat_vector1[i] = v_out1[i];
    }
    float abs_of_feat_vector1 = abs_of_vector(feat_vector1, feat_vector_len);

    //-----  m_ImageOutPtr   ??
    memset(m_ImageOutPtr,0,GtiGetOutputLength(m_Device));

    // handle imgData2
    cRet = convolution(imgData2, GTI_IMAGE_WIDTH * GTI_IMAGE_HEIGHT * 3);
    if(cRet != 0)
    {
        Ret = -103;// 103 ,convolution error
    }

    GtiClassifyFC(gClassify, m_ImageOutPtr, 1);

    // get feature vector
    float* v_out2 = GetFeatureVector(gClassify); // gtiClassify->m_Fc7_out
    float* feat_vector2 = new float[feat_vector_len]; // feature vector

    for (int i = 0; i < feat_vector_len; ++i) {
        feat_vector2[i] = v_out2[i];
    }

    float abs_of_feat_vector2 = abs_of_vector(feat_vector2, feat_vector_len);

    float cos_similarity = get_cos_sim(feat_vector1,feat_vector2, feat_vector_len, abs_of_feat_vector1, abs_of_feat_vector2);

    faceIdentifyProcessTime = (GetTickCount64()-m_IdentifyTimerStart);
    sprintf_s(charbuf, " get_cos_sim end faceIdentify: %5.2f ms", faceIdentifyProcessTime);
    //ALOG("--- %s", charbuf);

    m_IdentifyTimerEnd = GetTickCount64();
    faceIdentifyProcessTime = (m_IdentifyTimerEnd-m_IdentifyTimerStart);

    //sprintf_s(charbuf, "Two Cos Similarity : %5.2f,two pic faceIdentify: %5.2f ms", cos_similarity, faceIdentifyProcessTime);
    //ALOG("----------->>>  %s", charbuf);
    if (cos_similarity > 0.72f){
        Ret = 1;
    }
    return Ret;
}

int Samples::faceIdentify(const cv::Mat& imgData1,const cv::Mat& imgData2){
    int Ret = 0;
    char  charbuf[256];
    m_IdentifyTimerStart = GetTickCount64();

    // handle imgData1
    convolution(imgData1);

    // FC
    GtiClassifyFC(gClassify, m_ImageOutPtr, 1);

    // get feature vector
    float* v_out1 = GetFeatureVector(gClassify); // gClassify->m_Fc7_out
    float* feat_vector1 = new float[feat_vector_len]; // feature vector
    for (int i = 0; i < feat_vector_len; ++i) {
        feat_vector1[i] = v_out1[i];
    }
    float abs_of_feat_vector1 = abs_of_vector(feat_vector1, feat_vector_len);

    //-----  m_ImageOutPtr   ??
    memset(m_ImageOutPtr,0,GtiGetOutputLength(m_Device));

    // handle imgData2
    convolution(imgData2);

    GtiClassifyFC(gClassify, m_ImageOutPtr, 1);

    // get feature vector
    float* v_out2 = GetFeatureVector(gClassify); // gtiClassify->m_Fc7_out
    float* feat_vector2 = new float[feat_vector_len]; // feature vector

    for (int i = 0; i < feat_vector_len; ++i) {
        feat_vector2[i] = v_out2[i];
    }

    float abs_of_feat_vector2 = abs_of_vector(feat_vector2, feat_vector_len);

    float cos_similarity = get_cos_sim(feat_vector1,feat_vector2, feat_vector_len, abs_of_feat_vector1, abs_of_feat_vector2);

    faceIdentifyProcessTime = (GetTickCount64()-m_IdentifyTimerStart);
    sprintf_s(charbuf, " get_cos_sim end faceIdentify: %5.2f ms", faceIdentifyProcessTime);
    //ALOG("--- %s", charbuf);

    m_IdentifyTimerEnd = GetTickCount64();
    faceIdentifyProcessTime = (m_IdentifyTimerEnd-m_IdentifyTimerStart);

    //sprintf_s(charbuf, "Two Cos Similarity : %5.2f,two pic faceIdentify: %5.2f ms", cos_similarity, faceIdentifyProcessTime);
    //ALOG("----------->>>  %s", charbuf);
    if (cos_similarity > 0.72f){
        Ret = 1;
    }
    return Ret;
}