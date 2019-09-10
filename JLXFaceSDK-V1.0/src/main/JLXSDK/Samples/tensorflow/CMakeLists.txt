cmake_minimum_required(VERSION 3.7.1)

project(tensorflowlite)

add_compile_options(-std=c++11)
add_compile_options(-flax-vector-conversions)


set(JNI_JLXSDK_TFLITE ${CMAKE_SOURCE_DIR}/Samples/tensorflow/lite)

set(MINIMAL_SRCS  ${JNI_JLXSDK_TFLITE}/examples/minimal/minimal.cc)
set(PROFILER_SRCS ${JNI_JLXSDK_TFLITE}/profiling/time.cc)

file(GLOB PROFILE_SUMMARIZER_SRCS
        ${JNI_JLXSDK_TFLITE}/profiling/profile_summarizer.cc
        ${CMAKE_SOURCE_DIR}/core/util/stats_calculator.cc)


file(GLOB CORE_CC_EXCLUDE_SRCS
        ${JNI_JLXSDK_TFLITE}/*test.cc
        ${JNI_JLXSDK_TFLITE}/*/*test.cc
        ${JNI_JLXSDK_TFLITE}/*/*/*test.cc
        ${JNI_JLXSDK_TFLITE}/*/*/*/*test.cc
        ${JNI_JLXSDK_TFLITE}/kernels/*test_util.cc)


set(BUILD_WITH_NNAPT ${JNI_JLXSDK_TFLITE}/nnapi_delegate_disabled.cc)

file(GLOB WITH_MINIMAL_IOS_SRCS
        ${JNI_JLXSDK_TFLITE}/minimal_logging_android.cc
        ${JNI_JLXSDK_TFLITE}/minimal_logging_default.cc)


file(GLOB WITH_MINIMAL_NOT_IOS_SRCS
        ${JNI_JLXSDK_TFLITE}/minimal_logging_android.cc
        ${JNI_JLXSDK_TFLITE}/minimal_logging_ios.cc)

set(JNI_JLXSDK_TFLITE_Third_downloads 	${JNI_JLXSDK_TFLITE}/tools/make/downloads)
set(JNI_JLXSDK_TFLITE_Third_googletest  ${JJNI_JLXSDK_TFLITE_Third_downloads}/googletest)
set(JNI_JLXSDK_TFLITE_Third_flatbuffers ${JNI_JLXSDK_TFLITE_Third_downloads}/flatbuffers)



#source directory源文件目录
file(GLOB JNI_TENFLOW_LITE_ALL_SRC
        ${JNI_JLXSDK_TFLITE}/*.cc
        ${JNI_JLXSDK_TFLITE}/*.c
        ${JNI_JLXSDK_TFLITE}/c/*.c
        ${JNI_JLXSDK_TFLITE}/experimental/c/*.c
        ${JNI_JLXSDK_TFLITE}/experimental/c/*.cc
        ${JNI_JLXSDK_TFLITE}/core/*.cc
        ${JNI_JLXSDK_TFLITE}/core/api/*.cc
        ${JNI_JLXSDK_TFLITE}/kernels/*.cc
        ${JNI_JLXSDK_TFLITE}/kernels/internal/*.cc
        ${JNI_JLXSDK_TFLITE}/kernels/internal/optimized/*.cc
        ${JNI_JLXSDK_TFLITE}/kernels/internal/reference/*.cc
        ${PROFILER_SRCS}
        ${JNI_JLXSDK_TFLITE}/kernels/*.c
        ${JNI_JLXSDK_TFLITE}/kernels/internal/*.c
        ${JNI_JLXSDK_TFLITE}/kernels/internal/optimized/*.c
        ${JNI_JLXSDK_TFLITE}/kernels/internal/reference/*.c
        ${JNI_JLXSDK_TFLITE_Third_downloads}/farmhash/src/farmhash.cc
        ${JNI_JLXSDK_TFLITE_Third_downloads}/fft2d/fftsg.c
        ${JNI_JLXSDK_TFLITE_Third_googletest}/googletest/src/*.cc
        ${JNI_JLXSDK_TFLITE_Third_googletest}/googlemock/src/*.cc
        )


list(FILTER JNI_TENFLOW_LITE_ALL_SRC EXCLUDE REGEX "test.cc")

list(FILTER JNI_TENFLOW_LITE_ALL_SRC EXCLUDE REGEX "test_util.cc")

list(FILTER JNI_TENFLOW_LITE_ALL_SRC EXCLUDE REGEX "nnapi_delegate_disabled.cc")

list(FILTER JNI_TENFLOW_LITE_ALL_SRC EXCLUDE REGEX "minimal_logging_android.cc")

list(FILTER JNI_TENFLOW_LITE_ALL_SRC EXCLUDE REGEX "minimal_logging.cc")


set(TENSORFLOW_LITE_SRC ${JNI_TENFLOW_LITE_ALL_SRC})

list(LENGTH JNI_TENFLOW_LITE_ALL_SRC LITE_CC_SRCS_NUMBERS)

message(STATUS "LITE_CC_SRCS_NUMBERS=${LITE_CC_SRCS_NUMBERS}")

## ----
#ADD_LIBRARY(tensorflowlite SHARED ${TENSORFLOW_LITE_SRC})
#
#set_target_properties(tensorflowlite PROPERTIES LINKER_LANGUAGE CXX)


#target_link_libraries(tensorflowlite
#        android
#        atomic
#        c
#        z
#        m
#        dl
#        stdc++)