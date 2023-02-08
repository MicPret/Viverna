#ifndef VERNA_TEST_CAMERA_MOVEMENT_HPP
#define VERNA_TEST_CAMERA_MOVEMENT_HPP

#include <viverna/core/Input.hpp>
#include <viverna/core/Time.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/core/VivernaInitializer.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/Window.hpp>
#include <viverna/maths/MathUtils.hpp>
#include <viverna/maths/Vec2f.hpp>
#include <viverna/maths/Vec3f.hpp>

#include <utility>
#include <vector>

inline void CameraMovement(int seconds) {
    VERNA_LOGI("Running CameraMovement demo for " + std::to_string(seconds)
               + " seconds...");
    verna::VivernaInitializerRAII initializer;
    auto start = verna::Clock::now();
    auto end = start + verna::Seconds(seconds);
    auto now = start;

    verna::Mesh cube = verna::LoadPrimitiveMesh(verna::PrimitiveMeshType::Cube);
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
    verna::TextureId color =
        verna::LoadTextureFromColor(0.2f, 0.8f, 0.44f, 1.0f);
    verna::Material material;
    material.textures[0] = color;
    verna::ShaderId shader =
        verna::LoadShaderFromSource(vertex_src, fragment_src);

    verna::MouseListener mouse;
    constexpr float distance = 6.0f;
    constexpr float radius = 2.0f;
    constexpr float scale = 0.25f;
    constexpr verna::Vec3f sphere_pos = distance * verna::Vec3f::UnitZ();
    std::vector<verna::Transform> transforms;
    transforms.reserve(16);
    unsigned pos_x, pos_y;
    float timer;
    constexpr float timer_step = 0.06;
    float times = 1.0f;
    verna::KeyListener esc_key(verna::Key::Escape);

    verna::VivernaState& app_state = initializer.app_state;
    while (app_state.GetFlag(verna::VivernaState::RUNNING_FLAG)) {
        verna::DeltaTime<float, verna::Seconds> passed = now - start;
        mouse.Position(pos_x, pos_y);
        verna::Vec2f mouse_pos01(
            static_cast<float>(pos_x)
                / static_cast<float>(verna::WindowWidth()),
            static_cast<float>(pos_y)
                / static_cast<float>(verna::WindowHeight()));
        float rot_x =
            (mouse_pos01.y * 2.0f - 1.0f) * (verna::maths::Pi() * 0.5f);
        float rot_y = (mouse_pos01.x * 2.0f - 1.0f) * verna::maths::Pi();
        verna::Quaternion rotation =
            verna::Quaternion(verna::Vec3f::UnitY(), rot_y)
            * verna::Quaternion(verna::Vec3f::UnitX(), rot_x);
        verna::Camera::GetActive().rotation = rotation;
        // TODO position

        // spawn
        timer = passed.count();
        if (timer >= times * timer_step) {
            verna::Transform new_t;
            new_t.position = radius * verna::Vec3f::Random() + sphere_pos;
            new_t.rotation =
                verna::Quaternion((new_t.position - sphere_pos)
                                      .Cross(sphere_pos.Normalized())
                                      .Normalized(),
                                  verna::maths::Radians(90.0f));
            new_t.scale = scale * new_t.scale;
            transforms.push_back(std::move(new_t));
            times += 1.0f;
        }

        for (const auto& t : transforms)
            verna::Render(cube, material, t.GetMatrix(), shader);

        verna::Draw();

        now = verna::Clock::now();
        if (now >= end || esc_key.Pressed())
            app_state.SetFlag(verna::VivernaState::RUNNING_FLAG, false);
    }
    verna::FreeShader(shader);
    verna::FreeTexture(color);
}

#endif