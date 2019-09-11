cmake_minimum_required(VERSION 3.4.1)

set(JNI_JLXSDK_SRC_MAIN ${CMAKE_SOURCE_DIR})

set(JNI_JLXSDK_SRC ${JNI_JLXSDK_SRC_MAIN}/Samples)
set(JNI_JLXSDK_SRC_SAMPLE_INC $(JNI_JLXSDK_SRC)/Sample/Inc)
set(JNI_JLXSDK_INTERFACE_SRC ${JNI_JLXSDK_SRC_MAIN}/Samples/Android/jni)
set(JNI_JLXSDK_INCLUDE ${JNI_JLXSDK_SRC_MAIN}/Include)

set(JNI_JLXSDK_LIB ${JNI_JLXSDK_SRC_MAIN}/Lib/Android)

#添加coffee catch date:03-07
#set(COFFEE_CATCH_DIR ${JNI_JLXSDK_SRC}/coffeecatch)

include_directories(${JNI_JLXSDK_INCLUDE} ${JNI_JLXSDK_SRC}  ${JNI_JLXSDK_INTERFACE_SRC}/include)

#add opencv so path and include file
set(EXT_DIR ../../../../jlx_third_party/third_party)
set(OPENCV_PREBUILT ${EXT_DIR}/OpenCV-android-sdk/sdk/native)
LINK_DIRECTORIES(${OPENCV_PREBUILT}/libs/${ANDROID_ABI})
set(OPENCV_INCLUDE_FILE ${OPENCV_PREBUILT}/jni/include)
include_directories(${OPENCV_INCLUDE_FILE})
#end

#add ssl path
set(ssl_dir_path ${EXT_DIR}/../SSL)
set(ssl_include_path ${ssl_dir_path}/include)
set(ssl_libs_path ${CMAKE_SOURCE_DIR}/../../../../jlx_third_party/SSL/lib/jniLibs)
include_directories(${ssl_include_path})
#end

#add glog path
set(GLOG_PATH ${EXT_DIR}/miniglog/glog)
include_directories(${GLOG_PATH})
#end

#add sqlite3 so path
set(JNI_JLXSDK_SQL ${JNI_JLXSDK_SRC}/sqlite3so)
LINK_DIRECTORIES(${JNI_JLXSDK_SQL}/libs/${ANDROID_ABI})
#end



#add json head_hpp 2018-10-28
set(JNI_JLX_JSON_SRC_DIR     ../../../../jlx_third_party/Json/json_src)
set(JNI_JLX_JSON_INCLUDE_DIR ../../../../jlx_third_party/Json/json_include)

#add mtcnn include
set(JNI_MTCNN_INCLUDE_PATH ${JNI_JLXSDK_SRC}/facedetect/src/main/cpp)
#end
include_directories(${JNI_JLX_JSON_INCLUDE_DIR})

set(JNI_COMMON_JSON_SOURCES
    ${JNI_JLX_JSON_SRC_DIR}/json_reader.cpp
    ${JNI_JLX_JSON_SRC_DIR}/json_value.cpp
    ${JNI_JLX_JSON_SRC_DIR}/json_writer.cpp)
#end

LINK_DIRECTORIES(${JNI_JLXSDK_LIB}/${ANDROID_ABI})

SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS}")

#SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

#source directory源文件目录
file(GLOB JNI_SAMPLE_SRC
		${JNI_JLXSDK_SRC}/Sample/include/*.hpp
		${JNI_JLXSDK_SRC}/Sample/include/*.h
		${JNI_JLXSDK_SRC}/Sample/*.cc
		${JNI_JLXSDK_SRC}/Sample/*.cpp
		${JNI_JLXSDK_SRC}/facedetect/src/main/cpp/*.h
		${JNI_JLXSDK_SRC}/facedetect/src/main/cpp/*.cpp
		)


set(SAMPLE_FILE_COMPILE_CODE ${JNI_SAMPLE_SRC})
include_directories(${JNI_JLXSDK_SRC}/Sample/include/)

#添加libcrypto库
add_library(libcrypto STATIC IMPORTED)
set_target_properties(libcrypto
		PROPERTIES IMPORTED_LOCATION
		${ssl_libs_path}/${ANDROID_ABI}/lib/libcrypto.a)

#when you want use vulkan lib ,please minSdkVersion >= 24(android7.0 more than)
#add ncnn
if(${ANDROID_NATIVE_API_LEVEL} LESS 25)
	option(USE_VULKAN_FLAGS "not use vulkan lib" OFF)
else()
	option(USE_VULKAN_FLAGS "use vulkan lib" ON)
endif()

if(USE_VULKAN_FLAGS)
	include_directories(${JNI_MTCNN_INCLUDE_PATH}/include-vulkan)
	add_library(libncnn STATIC IMPORTED)
	set_target_properties(libncnn
		PROPERTIES IMPORTED_LOCATION
		${JNI_JLXSDK_SRC}/facedetect/src/main/jniLibs/vulkan-lib/${ANDROID_ABI}/libncnn.a)
else()
include_directories(${JNI_MTCNN_INCLUDE_PATH}/include)
add_library(libncnn STATIC IMPORTED)
set_target_properties(libncnn
        PROPERTIES IMPORTED_LOCATION
        ${JNI_JLXSDK_SRC}/facedetect/src/main/jniLibs/${ANDROID_ABI}/libncnn.a)

endif()


#add tensorflow lite head file
include_directories(${JNI_JLXSDK_SRC}/tensorflow)
include_directories(${JNI_JLXSDK_SRC}/tensorflow/lite/tools/make/downloads/flatbuffers/include)

# add 1.libfarmhash
add_library(libfarmhash STATIC IMPORTED)
set_target_properties(libfarmhash PROPERTIES IMPORTED_LOCATION
		${JNI_JLXSDK_SRC}/tfliteso/libs/${ANDROID_ABI}/external/farmhash_archive/libfarmhash.pic.a)

# add 2.libfft2d
add_library(libfft2d STATIC IMPORTED)
set_target_properties(libfft2d PROPERTIES IMPORTED_LOCATION
		${JNI_JLXSDK_SRC}/tfliteso/libs/${ANDROID_ABI}/external/fft2d/libfft2d.pic.a)

# add 3.libflatbuffers
add_library(libflatbuffers STATIC IMPORTED)
set_target_properties(libflatbuffers PROPERTIES IMPORTED_LOCATION
		${JNI_JLXSDK_SRC}/tfliteso/libs/${ANDROID_ABI}/external/flatbuffers/libflatbuffers.pic.a)


LINK_DIRECTORIES(${JNI_JLXSDK_SRC}/tfliteso/libs/${ANDROID_ABI})
#end

include_directories(${JNI_JLXSDK_INTERFACE_SRC}/include/)
# -----
add_library(jlx_classify SHARED
			${JNI_JLXSDK_INTERFACE_SRC}/jni_classify.cpp
			${JNI_JLXSDK_INTERFACE_SRC}/jni_detect.cpp
			${JNI_JLXSDK_INTERFACE_SRC}/jni_util.cpp
		    ${JNI_JLXSDK_SRC}/Sample/TensorflowModels.cpp
			${SAMPLE_FILE_COMPILE_CODE}
			${GLOG_PATH}/logging.cc
		    ${JNI_COMMON_JSON_SOURCES})

if(USE_VULKAN_FLAGS)
	target_link_libraries(jlx_classify
			libcrypto
			vulkan
			libncnn
			libfarmhash
			libfft2d
			libflatbuffers
			tensorflowlite
			android
			opencv_java3
			atomic
			c
			z
			m
			dl
			log
			stdc++
			gti_android
			sqlite3)
else()
target_link_libraries(jlx_classify
        libcrypto
        libncnn
		libfarmhash
		libfft2d
		libflatbuffers
		tensorflowlite
        android
        opencv_java3
        atomic
        c
        z
        m
        dl
        log
        stdc++
        gti_android
        sqlite3)
endif()

ADD_SUBDIRECTORY(Samples/filessl)



