cmake_minimum_required(VERSION 3.24.0)

project("viverna_ext" LANGUAGES CXX C)

if(CMAKE_SYSTEM_NAME STREQUAL Linux OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_subdirectory("${PROJECT_SOURCE_DIR}/desktop")
elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
    add_subdirectory("${PROJECT_SOURCE_DIR}/android")
endif()

add_subdirectory("${PROJECT_SOURCE_DIR}/stb")