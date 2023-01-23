cmake_minimum_required(VERSION 3.24.0)

project(glfw LANGUAGES C)

include(FetchContent)
FetchContent_Declare(${PROJECT_NAME}
    GIT_REPOSITORY https://github.com/glfw/glfw.git
)
FetchContent_MakeAvailable(${PROJECT_NAME})