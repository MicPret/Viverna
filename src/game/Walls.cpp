#include <game/core/Walls.hpp>

#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/Renderer.hpp>

namespace snake {
Walls::Walls(float distance) {
    for (auto& t : transforms)
        t.position.z = distance;
}

void Walls::Setup() {
    float distance = transforms[0].position.z;
    verna::Vec3f upper_left =
        verna::Camera::GetActive().ToWorldCoords(0, 0, distance);
    float dx = -upper_left.x;
    float dy = upper_left.y;
    float width = dx * 2.0f;
    float height = dy * 2.0f;
    transforms.back().position.z = distance + 1.0f;
    transforms.back().scale = verna::Vec3f(width, height, 0.1f);
    verna::Vec3f scaleV(0.5f, height, 1.0f);
    verna::Vec3f scaleH(width, 0.5f, 1.0f);
    transforms[0].position.x = -dx;
    transforms[0].scale = scaleV;
    transforms[1].position.y = -dy;
    transforms[1].scale = scaleH;
    transforms[2].position.x = dx;
    transforms[2].scale = scaleV;
    transforms[3].position.y = dy;
    transforms[3].scale = scaleH;
    mesh = verna::LoadPrimitiveMesh(verna::PrimitiveMeshType::Cube);
    for (size_t i = 0; i < colliders.size(); i++)
        colliders[i].Recalculate(mesh, transforms[i]);
    bg_material.textures[0] = verna::LoadTexture("bg.png");
    wall_material.textures[0] = verna::LoadTexture("wall.png");
}

void Walls::Dispose() {
    verna::FreeTexture(bg_material.textures[0]);
    verna::FreeTexture(wall_material.textures[0]);
}

void Walls::Render(verna::ShaderId shader) {
    size_t i;
    for (i = 0; i < colliders.size(); i++)
        verna::Render(mesh, wall_material, transforms[i].GetMatrix(), shader);
    verna::Render(mesh, bg_material, transforms[i].GetMatrix(), shader);

#ifdef VERNA_GAME_DEBUG
    for (const auto& c : colliders)
        verna::RenderDebug(c);
#endif
}

}  // namespace snake