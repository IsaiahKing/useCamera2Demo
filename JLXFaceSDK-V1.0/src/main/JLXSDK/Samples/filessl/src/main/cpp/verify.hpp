#ifndef __VERIFY__HPP
#define __VERIFY__HPP

#include <openssl/des.h>
#include <string>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>

std::string verifyFile(char* dst_file,char* keystring);
std::string ssl_main(char* agrv[]);
#endif