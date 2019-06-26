cmake_minimum_required(VERSION 3.4.1)
file(GLOB SOURCE src/main/cpp/*.cpp )
add_library(
        native-lib
        SHARED
        ${SOURCE})

include_directories("src/main/cpp/include")
find_library(
        log-lib
        log)


set(my_dir "${CMAKE_SOURCE_DIR}/src/main/cpp/libs/${ANDROID_ABI}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${my_dir}")

target_link_libraries(
        native-lib
        avfilter avformat    avcodec  avutil  swresample swscale
        z
        OpenSLES
        android
        log)