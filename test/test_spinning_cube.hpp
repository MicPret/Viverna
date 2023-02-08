#ifndef VERNA_TEST_SPINNING_CUBE_HPP
#define VERNA_TEST_SPINNING_CUBE_HPP

#include <viverna/core/Debug.hpp>
#include <viverna/core/Input.hpp>
#include <viverna/core/VivernaInitializer.hpp>
#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/RendererAPI.hpp>
#include <viverna/graphics/Shader.hpp>
#include <viverna/graphics/Window.hpp>
#include <viverna/maths/Quaternion.hpp>

inline void SpinningCube(int seconds) {
    VERNA_LOGI("Running SpinningCube demo for " + std::to_string(seconds)
               + " seconds...");
    verna::VivernaInitializerRAII initializer;

    std::string_view vertex_src =
        "layout(location = 0) in vec3 vPosition;\n"
        "layout(location = 1) in vec2 vTexCoords;\n"
        "out vec2 texCoords;\n\n"
        "void main() {\n"
        "   SetMeshIdx();\n"
        "   texCoords = vTexCoords;\n"
        "   gl_Position = camera.pv_matrix * MODEL_MATRIX * "
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

    auto start = verna::Clock::now();
    auto end = start + verna::Seconds(seconds);
    auto now = start;

    float t = 0.0f;
    verna::KeyListener esc_btn(verna::Key::Escape);
    while (now < end && !esc_btn.Pressed()) {
        verna::DeltaTime<float, verna::Seconds> since_start = now - start;
        t = since_start.count();
        verna::Mat4f transform =
            verna::Quaternion(verna::Vec3f::UnitY(), t).AsMatrix();
        /*
            verna::Mat4f::Rotation(verna::Vec3f::UnitY(), t);
            */
        verna::Render(cube, material, transform, shader);
        verna::Draw();
        now = verna::Clock::now();
    }
    verna::FreeShader(shader);
    verna::FreeTexture(cyan);
}

#endif