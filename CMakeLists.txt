cmake_minimum_required(VERSION 3.24.0 FATAL_ERROR)

# parse config.yaml
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/config.yaml" VERNA_CONFIG_FILE)
string(REGEX MATCH "APP_NAME: \"([A-Za-z0-9_ ]+)" _ ${VERNA_CONFIG_FILE})
set(VERNA_APP_NAME ${CMAKE_MATCH_1})
if(CMAKE_MATCH_COUNT LESS_EQUAL 0)
    message(FATAL_ERROR "Invalid app name in config.yaml!")
endif()
string(REPLACE " " "" VIVERNA_TARGET_NAME ${VERNA_APP_NAME})

# project
project(${VIVERNA_TARGET_NAME} LANGUAGES CXX C)

# default standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "=============================================================================================")
message(STATUS "App name: ${VERNA_APP_NAME}")
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

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    add_library(${VIVERNA_TARGET_NAME} SHARED "")
    target_compile_options(${VIVERNA_TARGET_NAME} PUBLIC "-fPIC")
    target_compile_definitions(${VIVERNA_TARGET_NAME} PUBLIC VERNA_ANDROID=1)
    file(COPY
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        DESTINATION
            "${CMAKE_CURRENT_SOURCE_DIR}/android/app/src/main")
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(${VIVERNA_TARGET_NAME} "")
    target_compile_definitions(${VIVERNA_TARGET_NAME} PUBLIC VERNA_DESKTOP=1)
    if(CMAKE_SYSTEM_NAME STREQUAL Linux)
        target_compile_definitions(${VIVERNA_TARGET_NAME} PUBLIC VERNA_LINUX=1)
    else()
        target_compile_definitions(${VIVERNA_TARGET_NAME} PUBLIC VERNA_WINDOWS=1)
    endif()
    file(COPY
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        DESTINATION
            "${CMAKE_BINARY_DIR}")
endif()
target_compile_options(${VIVERNA_TARGET_NAME} PRIVATE "-Wall;-fno-exceptions;-fno-rtti")
set_target_properties(${VIVERNA_TARGET_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "${VERNA_APP_NAME}"
)


# external includes and sources
add_subdirectory("${PROJECT_SOURCE_DIR}/ext")

# includes  
target_include_directories(${VIVERNA_TARGET_NAME} PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)

# sources
add_subdirectory("${PROJECT_SOURCE_DIR}/src")

# link
target_link_libraries(${VIVERNA_TARGET_NAME} PUBLIC
    stb
)
if(CMAKE_SYSTEM_NAME STREQUAL Android)
    target_link_libraries(${VIVERNA_TARGET_NAME} PUBLIC
        android
        EGL
        GLESv3
        log
        native_app_glue
        jnigraphics
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    target_link_libraries(${VIVERNA_TARGET_NAME} PUBLIC
        glad
        glfw
    )
endif()