#include <game/core/Fruit.hpp>

#include <viverna/core/Scene.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/physics/Collision.hpp>

namespace snake {
Fruit::Fruit(float distance) {
    transform.position.z = distance;
    transform.scale = verna::Vec3f(0.5f);
}

void Fruit::Setup() {
    mesh = verna::LoadPrimitiveMesh(verna::PrimitiveMeshType::Sphere);
    auto& tex_man = verna::Scene::GetActive().texture_manager;
    material.textures[verna::Material::DIFFUSE_INDEX] =
        tex_man.LoadTexture("fruit.png", {});
    material.textures[verna::Material::SPECULAR_INDEX] =
        tex_man.LoadTextureFromColor(verna::Color4u8(0, 0, 0, 255), {});
    collider.Recalculate(mesh, transform);
}

void Fruit::Dispose() {
    auto& tex_man = verna::Scene::GetActive().texture_manager;
    tex_man.FreeTexture(material.textures[verna::Material::DIFFUSE_INDEX]);
    tex_man.FreeTexture(material.textures[verna::Material::SPECULAR_INDEX]);
}

void Fruit::Render(verna::ShaderId shader) {
    verna::Render(mesh, material, transform, shader);

#ifdef VERNA_GAME_DEBUG
    verna::RenderDebug(collider);
#endif
}

void Fruit::Recalculate(BufferView<verna::BoundingSphere> sphere_cols,
                        BufferView<verna::BoundingBox> box_cols,
                        verna::Vec2f bounds_min,
                        verna::Vec2f bounds_max) {
    verna::BoundingBox box;
    box.Recalculate(mesh, transform);
    verna::Vec2f delta = bounds_max - bounds_min;

    while (true) {
        bool colliding = false;
        float x = bounds_min.x + delta.x * verna::maths::RandomFloatExclusive();
        float y = bounds_min.y + delta.y * verna::maths::RandomFloatExclusive();
        collider.Position() = verna::Vec3f(x, y, transform.position.z);
        for (const auto& s : sphere_cols) {
            if (SphereCollide(s, collider)) {
                colliding = true;
                break;
            }
        }
        if (colliding)
            continue;
        box.SetCenter(collider.Position());
        for (const auto& b : box_cols) {
            if (BoxCollide(b, box)) {
                colliding = true;
                break;
            }
        }
        if (!colliding) {
            transform.position = collider.Position();
            break;
        }
    }
}

}  // namespace snake