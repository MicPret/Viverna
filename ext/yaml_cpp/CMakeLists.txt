cmake_minimum_required(VERSION 3.24.0)

project(yaml_cpp LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(${PROJECT_NAME}
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
)
FetchContent_MakeAvailable(${PROJECT_NAME})