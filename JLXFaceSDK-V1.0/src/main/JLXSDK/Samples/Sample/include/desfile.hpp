#ifndef __DESFILE_HPP
#define __DESFILE_HPP

#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include "openssl/des.h"

namespace JLX{
    namespace SSL{
        class ModelFileCrypt {
        public:
            ModelFileCrypt();
            ModelFileCrypt(const ModelFileCrypt&)= delete;
            ModelFileCrypt& operator=(const ModelFileCrypt&)= delete;

            int crypt_block(
                    const uint8_t *buf,
                    int buf_size,
                    int enc,
                    std::vector<int> &result);

        private:
            std::string mkey = "1234cszx";
        };
    };
};

#endif
