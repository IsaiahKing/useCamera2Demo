#include "verify.hpp"
#include <android/log.h>
#define  JNI_LOG_SSL_TAG    "JLX_SSL_JNI"

/**
*加解密 字符cpp
*/
//dst_file    加密后的文件名
//keystring   从api中读取的字符串
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,JNI_LOG_SSL_TAG,__VA_ARGS__)

std::string verifyFile(char* dst_file,char* keystring)
{
    if(dst_file == nullptr){
        return nullptr;
    }

    FILE *dst_fp = fopen(dst_file, "rb");
    if(dst_fp == nullptr){
        return nullptr;
    }
    int   offset = 0 ;
    fseek(dst_fp,offset,SEEK_END);

    //获取文件长度;
    long length= ftell(dst_fp);
    unsigned char *output = new unsigned char[length];
    unsigned char *input  = new unsigned char[length+1];
    memset(input,0,length+1);

    DES_cblock key;
    DES_key_schedule key_schedule;

    fseek(dst_fp,offset,SEEK_SET);
    size_t real_len = fread(output, 1, length, dst_fp);

    //生成一个 key
    DES_string_to_key(keystring, &key);
    if (DES_set_key_checked(&key, &key_schedule) != 0) {
        printf("convert to key_schedule failed.\n");
        return nullptr;
    }

    DES_cblock ivec;

    memset((char*)&ivec, 0, sizeof(ivec));

    //printf("fread: %d\r\n",real_len);

//    for (int i = 0; i < real_len; ++i)
//        printf("%02x ", output[i]);

    //printf("\n");


    memset((char*)&ivec, 0, sizeof(ivec));
    DES_ncbc_encrypt(output, input, length, &key_schedule, &ivec, DES_DECRYPT);

    //printf("input:  %s \r\n",input);
    //ALOG("input contents : %s",input);
    std::string result("");

    if(strlen((const char*)input) > 0){
        result = (const char*)input;
    }
    if(output != NULL){
        delete []output;
    }
    if(input != NULL){
        delete []input;
    }
    fclose(dst_fp);

    return result;
}
std::string ssl_main(char* agrv[]){
    return verifyFile(agrv[0],agrv[1]);
}
