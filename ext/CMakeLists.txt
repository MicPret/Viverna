cmake_minimum_required(VERSION 3.24.0)

project("${VIVERNA_TARGET_NAME}_ext" LANGUAGES C)

if(CMAKE_SYSTEM_NAME STREQUAL Linux OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    #glad
    add_subdirectory("${PROJECT_SOURCE_DIR}/glad")
    #glfw
    include(FetchContent)
    FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
    )
    FetchContent_MakeAvailable(glfw)
elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
    target_include_directories(${VIVERNA_TARGET_NAME} PUBLIC
        "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue"
    )
    add_library(native_app_glue OBJECT
        "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")
    # TODO could delete???
    # Export ANativeActivity_onCreate(),
    # Refer to: https://github.com/android-ndk/ndk/issues/381.
    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
endif()