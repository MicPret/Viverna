cmake_minimum_required(VERSION 3.24.0)

project(viverna_engine LANGUAGES CXX C)
set(VIVERNA_TARGET_NAME ${PROJECT_NAME})

message(STATUS "=============================================================================================")
message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")
message(STATUS "System version: ${CMAKE_SYSTEM_VERSION}")
if (CMAKE_SYSTEM_NAME STREQUAL Android)
    message(STATUS "NDK path: ${CMAKE_ANDROID_NDK}")
    message(STATUS "Android ABI: ${CMAKE_ANDROID_ARCH_ABI}")
    message(STATUS "Android STL version: ${CMAKE_ANDROID_STL_TYPE}")
endif()
message(STATUS "C compiler:   ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "=============================================================================================")

add_library(${VIVERNA_TARGET_NAME} OBJECT "")
set_target_properties(${VIVERNA_TARGET_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    COMPILE_WARNING_AS_ERROR ON
)
target_compile_options(${VIVERNA_TARGET_NAME} PRIVATE "-Wall;-fno-exceptions;-fno-rtti")
if (CMAKE_SYSTEM_NAME STREQUAL Android)
    target_compile_definitions(${VIVERNA_TARGET_NAME} PUBLIC VERNA_ANDROID=1)
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    target_compile_definitions(${VIVERNA_TARGET_NAME} PUBLIC VERNA_DESKTOP=1)
endif()

# external includes and sources
add_subdirectory("${PROJECT_SOURCE_DIR}/ext")

# includes
target_include_directories(${VIVERNA_TARGET_NAME} PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)

# sources
add_subdirectory("${PROJECT_SOURCE_DIR}/src")

# link
if(CMAKE_SYSTEM_NAME STREQUAL Android)
    target_link_libraries(${VIVERNA_TARGET_NAME} PUBLIC
        android
        EGL
        GLESv3
        log
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    target_link_libraries(${VIVERNA_TARGET_NAME} PUBLIC
        glad
        glfw
    )
endif()