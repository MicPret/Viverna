cmake_minimum_required(VERSION 3.24.0)

project("viverna_ext_desktop" LANGUAGES C)

#glad
add_subdirectory("${PROJECT_SOURCE_DIR}/glad")
#glfw
add_subdirectory("${PROJECT_SOURCE_DIR}/glfw")