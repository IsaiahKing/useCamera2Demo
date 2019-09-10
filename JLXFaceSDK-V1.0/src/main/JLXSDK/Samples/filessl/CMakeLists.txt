# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

#include头文件目录
include_directories(src/main/cpp/include
        src/main/cpp/)

set(jlx_third_party_path ${CMAKE_SOURCE_DIR}/../../../../jlx_third_party)

#add glog path
set(GLOG_PATH ${jlx_third_party_path}/third_party/miniglog/glog)
include_directories(${GLOG_PATH})
#end


#source directory源文件目录
file(GLOB JNISSL_SRC
        src/main/cpp/*.hpp
        src/main/cpp/*.cpp
        ${GLOG_PATH}/*.h
        ${GLOG_PATH}/*.cc)

set(FILESSL_COMPILE_CODE ${JNISSL_SRC})

#添加libcrypto库
#add_library(libcrypto STATIC IMPORTED)
#set_target_properties(libcrypto
#        PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/Samples/filessl/src/main/jniLibs/${ANDROID_ABI}/lib/libcrypto.a)


#编译为动态库
add_library(jlx_filessl SHARED ${FILESSL_COMPILE_CODE})

#添加工程所依赖的库
find_library(log-lib log)

target_link_libraries(jlx_filessl
        libcrypto
        jnigraphics
        z
        ${log-lib})
