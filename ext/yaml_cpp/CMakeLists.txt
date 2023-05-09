cmake_minimum_required(VERSION 3.24.0)

project(yaml-cpp LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(${PROJECT_NAME}
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
)
FetchContent_MakeAvailable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC "${${PROJECT_NAME}_SOURCE_DIR}/include")