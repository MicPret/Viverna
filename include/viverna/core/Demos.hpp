#ifndef VERNA_DEMOS_HPP
#define VERNA_DEMOS_HPP

#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/Shader.hpp>
#include "Debug.hpp"

#include <chrono>

namespace verna {

inline void SpinningCubeDemo() {
    verna::InitializeRenderer();
    std::string_view vertex_src =
        "layout(location = 0) in vec3 vPosition;\n"
        "layout(location = 1) in vec2 vTexCoords;\n"
        "out vec2 texCoords;\n\n"
        "void main() {\n"
        "   SetMeshIdx();\n"
        "   texCoords = vTexCoords;\n"
        "   gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * MODEL_MATRIX * "
        "vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);\n"
        "}";

    std::string_view fragment_src =
        "in vec2 texCoords;\n"
        "out vec4 outputColor;\n\n"
        "void main() {\n"
        "  outputColor = texture(TEXTURE0, texCoords);\n"
        "}";
    verna::TextureId cyan =
        verna::LoadTextureFromColor(0.1f, 0.95f, 0.9f, 1.0f);
    verna::Material material;
    material.textures[0] = cyan;

    verna::ShaderId shader =
        verna::LoadShaderFromSource(vertex_src, fragment_src);
    verna::Mesh cube = verna::LoadPrimitiveMesh(verna::PrimitiveMeshType::Cube);
    verna::Camera& camera = verna::Camera::GetActive();
    camera.position.z = -3.0f;

    auto now = std::chrono::steady_clock::now();
    auto end = now + std::chrono::seconds(10);

    while (now < end) {
        std::chrono::duration<float, std::chrono::seconds::period> remaining =
            end - now;
        verna::Mat4f transform = verna::Mat4f::Rotation(
            verna::Vec3f(0.0f, 1.0f, 0.0f), remaining.count());
        verna::Render(cube, material, transform, shader);
        verna::Draw();
        now = std::chrono::steady_clock::now();
    }
    verna::FreeShader(shader);
    verna::FreeTexture(cyan);
    verna::TerminateRenderer();
}
}  // namespace verna

#endif