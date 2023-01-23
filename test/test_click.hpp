#ifndef VERNA_TEST_CLICK_HPP
#define VERNA_TEST_CLICK_HPP

#include <viverna/core/Debug.hpp>
#include <viverna/core/Input.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/core/VivernaInitializer.hpp>
#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/RendererAPI.hpp>
#include <viverna/graphics/Shader.hpp>
#include <viverna/graphics/Window.hpp>

#include <utility>

inline void Click(int seconds) {
    VERNA_LOGI("Running Click demo for " + std::to_string(seconds)
               + " seconds...");
    verna::VivernaInitializerRAII initializer;
    if (initializer.app_state.GetFlag(verna::VivernaState::ERROR_FLAG)) {
        VERNA_LOGE("An error occured???? HOW?????");
        return;
    }

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
    std::array<verna::Mesh, 2> meshes;
    meshes[0] = verna::LoadPrimitiveMesh(verna::PrimitiveMeshType::Cube);
    meshes[1] = verna::LoadPrimitiveMesh(verna::PrimitiveMeshType::Pyramid);
    verna::Camera& camera = verna::Camera::GetActive();
    camera.position.z = -3.0f;

    auto now = verna::Clock::now();
    auto end = now + verna::Seconds(seconds);

    verna::ClickListener mouse_btn;
    bool prev_pressed = false;
    std::vector<verna::Transform> transforms;
    while (now < end) {
        verna::DeltaTime<float, verna::Seconds> remaining = end - now;
        unsigned x, y;
        if (mouse_btn.Pressed(x, y)) {
            if (!prev_pressed) {
                verna::Transform transform;
                transform.position = camera.ToWorldCoords(x, y, 3.0f);
                transform.scale = verna::Vec3f(0.5f, 0.5f, 0.5f);
                transforms.push_back(std::move(transform));
                prev_pressed = true;
            }
        } else {
            prev_pressed = false;
        }
        verna::Quaternion rotation(verna::Vec3f::UnitY(), remaining.count());
        for (size_t i = 0; i < transforms.size(); i++) {
            const verna::Mesh& mesh = meshes[i % 2];
            verna::Transform& transform = transforms[i];
            transform.rotation = rotation;
            verna::Render(mesh, material, transform.GetMatrix(), shader);
        }
        verna::Draw();
        now = verna::Clock::now();
    }
    verna::FreeShader(shader);
    verna::FreeTexture(cyan);
}

#endif