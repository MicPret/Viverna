cmake_minimum_required(VERSION 3.24.0)

project("native_app_glue" LANGUAGES C)

add_library(${PROJECT_NAME} OBJECT
    "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c"
)
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue"
)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")