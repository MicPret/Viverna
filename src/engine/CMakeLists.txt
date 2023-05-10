cmake_minimum_required(VERSION 3.24.0 FATAL_ERROR)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    set(VERNA_ENGINE_PLATFORM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/android")
elseif(CMAKE_SYSTEM_NAME STREQUAL Windows OR CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(VERNA_ENGINE_PLATFORM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/desktop")
endif()

#private headers
target_sources(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/ResourceTracker.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ShaderBucketMapper.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/UniformBuffer.hpp"
)
    
target_sources(${PROJECT_NAME} PRIVATE
    "${VERNA_ENGINE_PLATFORM_PATH}/Assets.cpp"
    "${VERNA_ENGINE_PLATFORM_PATH}/Debug.cpp"
    "${VERNA_ENGINE_PLATFORM_PATH}/Input.cpp"
    "${VERNA_ENGINE_PLATFORM_PATH}/RendererAPI.cpp"
    "${VERNA_ENGINE_PLATFORM_PATH}/Window.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BoundingBox.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BoundingSphere.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Collision.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ComponentBuffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Entity.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/EntityName.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Family.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Image.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Mesh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Mat4f.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/MathUtils.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/MaterialSerializer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Model.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PointLightData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Quaternion.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/QuaternionSerializer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Scene.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ShaderSerializer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/System.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Texture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Transform.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/TransformSerializer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/TypeId.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/UniformBuffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Vec3f.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Vec3fSerializer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Vec4fSerializer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/VivernaInitializer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/World.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/WorldSerializer.cpp"
)
