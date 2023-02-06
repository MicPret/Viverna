#ifndef VERNA_TEST_COLLISION_HPP
#define VERNA_TEST_COLLISION_HPP

#include <viverna/core/VivernaInitializer.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Texture.hpp>
#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Shader.hpp>
#include <viverna/physics/Collision.hpp>

namespace detail {
void CollisionSphere(int seconds);
void CollisionBox(int seconds);
}  // namespace detail

inline void Collision(int seconds) {
    VERNA_LOGI("Running Collision demo for " + std::to_string(seconds)
               + " seconds...");
    verna::VivernaInitializerRAII init;
    int a = seconds / 2;
    detail::CollisionSphere(seconds - a);
    detail::CollisionBox(a);
}

namespace detail {
inline void CollisionSphere(int seconds) {
    auto start = verna::Clock::now();
    auto end = start + verna::Seconds(seconds);
    auto now = start;
    auto prev = now;

    verna::Mesh mesh = verna::LoadPrimitiveMesh(verna::PrimitiveMeshType::Cube);
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
        verna::LoadTextureFromColor(0.2f, 0.4f, 0.499f, 1.0f);
    verna::Material material;
    material.textures[0] = color;
    verna::ShaderId shader =
        verna::LoadShaderFromSource(vertex_src, fragment_src);

    constexpr float distance = 6.0f;
    constexpr verna::Vec3f center = distance * verna::Vec3f::UnitZ();
    verna::BoundingSphere central_collider;
    verna::Transform t;
    t.position = center;
    central_collider.Recalculate(mesh, t);
    float h = central_collider.Radius() / verna::maths::Sqrt(2.0f);
    central_collider.Radius() = 0.5 * (central_collider.Radius() + h);
    std::array<verna::Vec3f, 4> positions;
    positions.fill(center);
    positions[0].x -= distance;
    positions[0].y -= distance;
    positions[1].x += distance;
    positions[1].y -= distance;
    positions[2].x += distance;
    positions[2].y += distance;
    positions[3].x -= distance;
    positions[3].y += distance;
    std::array<verna::BoundingSphere, positions.size()> colliders;
    for (size_t i = 0; i < positions.size(); i++) {
        colliders[i].Position() = positions[i];
        colliders[i].Radius() = central_collider.Radius();
    }

    constexpr float speed = 4.0f;
    while (now < end) {
        verna::DeltaTime<float, verna::Seconds> passed = now - prev;
        for (auto& p : positions) {
            p = p + speed * passed.count() * (center - p).Normalized();
        }

        verna::Collision col;
        for (size_t i = 0; i < positions.size(); i++) {
            colliders[i].Position() = positions[i];
            if (verna::SphereCollide(colliders[i], central_collider, col)) {
                positions[i] = positions[i] - col.magnitude * col.direction;
                colliders[i].Position() = positions[i];
            }
        }

        for (const auto& p : positions) {
            t.position = p;
            verna::Render(mesh, material, t.GetMatrix(), shader);
        }
        t.position = center;
        verna::Render(mesh, material, t.GetMatrix(), shader);
        for (const auto& sc : colliders) {
            verna::RenderDebug(sc);
        }
        verna::RenderDebug(central_collider);

        verna::Draw();

        prev = now;
        now = verna::Clock::now();
    }
    verna::FreeShader(shader);
    verna::FreeTexture(color);
}

inline void CollisionBox(int seconds) {
    auto start = verna::Clock::now();
    auto end = start + verna::Seconds(seconds);
    auto now = start;
    auto prev = now;

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
        verna::LoadTextureFromColor(0.2f, 0.4f, 0.499f, 1.0f);
    verna::Material material;
    material.textures[0] = color;
    verna::ShaderId shader =
        verna::LoadShaderFromSource(vertex_src, fragment_src);

    constexpr float distance = 6.0f;
    constexpr verna::Vec3f center = distance * verna::Vec3f::UnitZ();
    verna::Transform center_transform;
    center_transform.position = center;
    verna::BoundingBox center_box;
    center_box.Recalculate(cube, center_transform);
    std::array<verna::Vec3f, 4> positions;
    positions.fill(center);
    positions[0].x -= distance;
    positions[0].y -= distance;
    positions[1].x += distance;
    positions[1].y -= distance;
    positions[2].x += distance;
    positions[2].y += distance;
    positions[3].x -= distance;
    positions[3].y += distance;
    std::array<verna::BoundingBox, 4> boxes;

    constexpr float speed = 4.0f;
    verna::Transform t;
    while (now < end) {
        verna::DeltaTime<float, verna::Seconds> passed = now - prev;
        for (size_t i = 0; i < positions.size(); i++) {
            verna::Vec3f& p = positions[i];
            p = p + speed * passed.count() * (center - p).Normalized();
            t.position = p;
            boxes[i].Recalculate(cube, t);
        }

        verna::Collision col;
        for (size_t i = 0; i < positions.size(); i++) {
            if (verna::BoxCollide(boxes[i], center_box, col)) {
                positions[i] = positions[i] - col.magnitude * col.direction;
            }
        }

        for (const auto& p : positions) {
            t.position = p;
            verna::Render(cube, material, t.GetMatrix(), shader);
        }
        t.position = center;
        verna::Render(cube, material, t.GetMatrix(), shader);

        for (const verna::BoundingBox& box : boxes)
            verna::RenderDebug(box);
        verna::RenderDebug(center_box);

        verna::Draw();

        prev = now;
        now = verna::Clock::now();
    }
    verna::FreeShader(shader);
    verna::FreeTexture(color);
}
}  // namespace detail
#endif