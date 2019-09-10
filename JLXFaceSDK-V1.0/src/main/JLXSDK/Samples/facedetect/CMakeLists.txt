# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
if(CMAKE_SYSTEM_NAME STREQUAL  “Android”)
    message("------------------------------")
endif()

cmake_minimum_required(VERSION 3.4.1)

#include头文件目录
include_directories(src/main/cpp/)

set(jlx_third_party_path ${CMAKE_SOURCE_DIR}/../../../../jlx_third_party)

set(OPENCV_DIR ${jlx_third_party_path}/third_party/OpenCV-android-sdk)
set(OPENCV_PREBUILT ${OPENCV_DIR}/sdk/native)

#添加 opencv 头文件
include_directories(${OPENCV_PREBUILT}/jni/include)
#添加opencv so
LINK_DIRECTORIES(${OPENCV_PREBUILT}/libs/${ANDROID_ABI})

#add glog path
set(GLOG_PATH ${jlx_third_party_path}/third_party/miniglog/glog)
include_directories(${GLOG_PATH})
#end

#add
option(USE_VULKAN_FLAGS "use vulkan lib" OFF)
if(USE_VULKAN_FLAGS)
    include_directories(src/main/cpp/include-vulkan)
    #source directory源文件目录
    file(GLOB MTCNN_SRC src/main/cpp/*.h
            src/main/cpp/*.cpp ${GLOG_PATH}/*.h ${GLOG_PATH}/*.cc)
    set(MTCNN_COMPILE_CODE ${MTCNN_SRC})

    #添加ncnn库
    add_library(libncnn STATIC IMPORTED)
    set_target_properties(libncnn
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/Samples/facedetect/src/main/jniLibs/vulkan-lib/${ANDROID_ABI}/libncnn.a)

    #编译为动态库
    add_library(mtcnn SHARED ${MTCNN_COMPILE_CODE})

    #添加工程所依赖的库
    find_library(log-lib log)

    target_link_libraries(mtcnn
            vulkan
            libncnn
            jnigraphics
            opencv_java3
            z
            ${log-lib})
else()
    include_directories(src/main/cpp/include)
    #source directory源文件目录
    file(GLOB MTCNN_SRC src/main/cpp/*.h
            src/main/cpp/*.cpp ${GLOG_PATH}/*.h ${GLOG_PATH}/*.cc)
    set(MTCNN_COMPILE_CODE ${MTCNN_SRC})

    #添加ncnn库
    add_library(libncnn STATIC IMPORTED)
    set_target_properties(libncnn
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/Samples/facedetect/src/main/jniLibs/${ANDROID_ABI}/libncnn.a)


    #编译为动态库
    add_library(mtcnn SHARED ${MTCNN_COMPILE_CODE})

    #添加工程所依赖的库
    find_library(log-lib log)

    target_link_libraries(mtcnn
            libncnn
            jnigraphics
            opencv_java3
            z
            ${log-lib})
endif()
#end

