cmake_minimum_required(VERSION 3.24.0)

#glad
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glad")
#glfw
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glfw")
#imgui
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/imgui")
target_link_libraries(imgui PUBLIC glfw)