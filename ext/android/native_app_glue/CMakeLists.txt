cmake_minimum_required(VERSION 3.24.0)

project("native_app_glue_header" LANGUAGES C)

add_library(${PROJECT_NAME} INTERFACE
)
target_include_directories(${PROJECT_NAME} INTERFACE
    "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue"
)
# target_link_options(${PROJECT_NAME} PUBLIC
#     "-u ANativeActivity_onCreate"
# )
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")