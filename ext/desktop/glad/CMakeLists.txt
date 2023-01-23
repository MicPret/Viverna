cmake_minimum_required(VERSION 3.24.0)

project(glad LANGUAGES C)

add_library(${PROJECT_NAME} STATIC
    "${PROJECT_SOURCE_DIR}/src/gl.c"
)
target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)