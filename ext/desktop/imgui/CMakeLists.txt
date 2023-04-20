cmake_minimum_required(VERSION 3.24.0)

project(imgui LANGUAGES CXX)

add_library(${PROJECT_NAME} OBJECT
    "${PROJECT_SOURCE_DIR}/BuildImgui.cpp"
)
target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_SOURCE_DIR}/imgui"
    "${PROJECT_SOURCE_DIR}"
)