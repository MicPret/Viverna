#ifndef VERNA_TEST_INTERPOLATION_HPP
#define VERNA_TEST_INTERPOLATION_HPP

#include <viverna/core/Debug.hpp>
#include <viverna/core/Input.hpp>
#include <viverna/core/Time.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/core/VivernaInitializer.hpp>
#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/RendererAPI.hpp>
#include <viverna/graphics/Shader.hpp>
#include <viverna/graphics/Window.hpp>

inline void Interpolation(int seconds) {
    VERNA_LOGI("Running Interpolation demo for " + std::to_string(seconds)
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
    verna::TextureId kinda_red =
        verna::LoadTextureFromColor(0.47f, 0.23f, 0.01f, 1.0f);
    verna::Material material;
    material.textures[0] = kinda_red;

    verna::ShaderId shader =
        verna::LoadShaderFromSource(vertex_src, fragment_src);
    verna::Mesh pyramid =
        verna::LoadPrimitiveMesh(verna::PrimitiveMeshType::Pyramid);
    verna::Camera& camera = verna::Camera::GetActive();

    constexpr float camera_start = -2.0f;
    camera.position = verna::Vec3f(0.0f, 0.0f, camera_start);
    verna::Vec3f pos_start(-4.3f, 3.4f, 3.5f);
    verna::Vec3f pos_end(4.3f, -3.4f, 18.5f);
    float half_seconds = static_cast<float>(seconds) * 0.5f;

    auto start = verna::Clock::now();
    auto end = start + verna::Seconds(seconds);
    auto now = start;

    verna::KeyListener esc_btn(verna::Key::Escape);
    while (now < end && !esc_btn.Pressed()) {
        verna::DeltaTime<float, verna::Seconds> passed = now - start;
        float t = passed.count() / static_cast<float>(seconds);
        float t2 = verna::maths::Min(passed.count() / half_seconds, 1.0f);

        verna::Transform transform;
        transform.position = verna::Vec3f::Lerp(pos_start, pos_end, t);

        verna::Vec3f rot_axis = -verna::Vec3f::UnitZ();
        float rot_radians =
            verna::maths::Lerp(0.1f, verna::maths::Radians(90.0f), t2);
        transform.rotation = verna::Quaternion(rot_axis, rot_radians);

        verna::Render(pyramid, material, transform.GetMatrix(), shader);
        transform.position = transform.position + verna::Vec3f::UnitX();
        verna::Render(pyramid, material, transform.GetMatrix(), shader);

        verna::Draw();
        now = verna::Clock::now();
    }
    verna::FreeShader(shader);
    verna::FreeTexture(kinda_red);
}

#endif