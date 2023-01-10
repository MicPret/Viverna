cmake_minimum_required(VERSION 3.24.0)

project("${VIVERNA_TARGET_NAME}_ext" LANGUAGES C)

if(CMAKE_SYSTEM_NAME STREQUAL Linux OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    #glad
    add_subdirectory("${PROJECT_SOURCE_DIR}/glad")
    #glfw
    include(FetchContent)
    FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
    )
    FetchContent_MakeAvailable(glfw)
    #stb
endif()
add_subdirectory("${PROJECT_SOURCE_DIR}/stb")