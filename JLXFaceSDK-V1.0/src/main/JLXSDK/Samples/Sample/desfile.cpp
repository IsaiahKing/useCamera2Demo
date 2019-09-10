#include <android/log.h>
#include "desfile.hpp"
#define  JNI_SSLFILE_LOG_TAG    "JLX_SSL_JNI"


namespace JLX{
    namespace SSL{
        ModelFileCrypt::ModelFileCrypt(){
        }
        //数据整块    读取加解密
        //fp         文件指针
        int ModelFileCrypt::crypt_block(
                const uint8_t *buf,
                int   buf_size,
                int   enc,
                std::vector<int>& result){

            const static int count = 8;
            DES_key_schedule        mkey_sch;

            DES_set_key_unchecked((const_DES_cblock *)mkey.c_str(), &mkey_sch);

            DES_cblock              ivec;
            memset((char*)&ivec, 0, sizeof(ivec));

            assert(buf_size % count == 0);

            result.clear();

            for(int i = 0;i < buf_size/count;i++)
            {
                unsigned char  tmp[8]={0};
                DES_ncbc_encrypt(buf + i*count, tmp, count, &mkey_sch, &ivec, enc);


                int *p = (int*)&tmp;
                for (int j = 0;j < sizeof(p);j++){
                    if(j < 2){
                        result.push_back((p[j]));
                    }
                }
            }
            return 0;
        }
    };
};


