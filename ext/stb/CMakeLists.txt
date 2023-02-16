cmake_minimum_required(VERSION 3.24.0)

project(stb LANGUAGES C)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
    "${PROJECT_SOURCE_DIR}/include"
)
