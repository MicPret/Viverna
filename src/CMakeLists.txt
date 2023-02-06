cmake_minimum_required(VERSION 3.24.0)
project("${VIVERNA_TARGET_NAME}_src" LANGUAGES CXX C)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    set("${PROJECT_NAME}_platform_path" "${CMAKE_CURRENT_SOURCE_DIR}/android")
elseif(CMAKE_SYSTEM_NAME STREQUAL Windows OR CMAKE_SYSTEM_NAME STREQUAL Linux)
    set("${PROJECT_NAME}_platform_path" "${CMAKE_CURRENT_SOURCE_DIR}/desktop")
endif()

#private headers
target_sources(${VIVERNA_TARGET_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/ResourceTracker.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ShaderBucketMapper.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/UniformBuffer.hpp"
)
    
    target_sources(${VIVERNA_TARGET_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/BoundingBox.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BoundingSphere.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Collision.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Mesh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ShaderCommonCode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Mat4f.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/MathUtils.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Transform.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Texture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Quaternion.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/UniformBuffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/VivernaInitializer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Vec3f.cpp"
    "${${PROJECT_NAME}_platform_path}/Debug.cpp"
    "${${PROJECT_NAME}_platform_path}/Input.cpp"
    "${${PROJECT_NAME}_platform_path}/RendererAPI.cpp"
    "${${PROJECT_NAME}_platform_path}/Window.cpp"
)