#include <game/core/Snake.hpp>

#include <viverna/core/Input.hpp>
#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/maths/MathUtils.hpp>
#include <viverna/maths/Vec3f.hpp>
#include <viverna/physics/Collision.hpp>

#include <cmath>

namespace snake {

Snake::Snake(float distance_) : distance(distance_) {
    transforms.reserve(2);
    colliders.reserve(2);
}

void Snake::Setup() {
    mesh = verna::LoadPrimitiveMesh(verna::PrimitiveMeshType::Sphere);
    transforms.resize(2);
    transforms[0].position = distance * verna::Vec3f::UnitZ();
    transforms[0].rotation =
        verna::Quaternion(-verna::Vec3f::UnitX(), verna::maths::Radians(90.0f));
    transforms[0].scale = verna::Vec3f(0.5f);
    transforms[1] = transforms[0];
    transforms[1].position =
        transforms[0].position - 0.1f * transforms[0].Forward();
    colliders.resize(2);
    colliders[0].Recalculate(mesh, transforms[0]);
    colliders[1].Recalculate(mesh, transforms[1]);

    material.textures[0] = verna::LoadTexture("snake.png");
}

void Snake::Dispose() {
    verna::FreeTexture(material.textures[0]);
    transforms.clear();
    colliders.clear();
}

void Snake::Render(verna::ShaderId shader) {
    for (const auto& t : transforms)
        verna::Render(mesh, material, t.GetMatrix(), shader);

#ifdef VERNA_GAME_DEBUG
    for (const auto& c : colliders)
        verna::RenderDebug(c);
#endif
}

void Snake::MoveTowards(float dt, const verna::Vec3f& target) {
    auto& head = transforms.front();
    auto forward = Direction();
    float interp = verna::maths::Min(dt * rot_speed, 1.0f);
    auto direction =
        verna::Vec3f::Lerp(forward, (target - HeadPosition()).Normalized(),
                           interp)
            .Normalized();
    float cos_angle = forward.Dot(direction);
    if (cos_angle < 0.999999f) {
        float angle =
            cos_angle <= -1.0f ? verna::maths::Pi() : std::acos(cos_angle);
        verna::Quaternion rot(forward.Cross(direction).Normalized(), angle);
        head.rotation = rot * head.rotation;
        forward = Direction();
    }
    float scaled_speed = dt * mov_speed;
    auto new_pos = HeadPosition() + scaled_speed * forward;
    for (size_t i = transforms.size() - 1; i > 0; i--) {
        auto dir =
            (transforms[i - 1].position - transforms[i].position).Normalized();
        transforms[i].position += scaled_speed * dir;
    }
    head.position = new_pos;
    Adjust();
}

void Snake::Adjust() {
    // update collider positions
    for (size_t i = 0; i < transforms.size(); i++)
        colliders[i].Position() = transforms[i].position;
    verna::Collision c;
    // first part of the tail
    if (SphereCollide(colliders[1], colliders[0], c)) {
        verna::Vec3f pos = transforms[1].position - (c.magnitude * c.direction);
        transforms[1].position = pos;
        colliders[1].Position() = pos;
    }
    // handle tail-tail collision
    for (size_t i = 2; i < colliders.size(); i++) {
        for (size_t j = 1; j < i; j++) {
            if (SphereCollide(colliders[i], colliders[j], c)) {
                verna::Vec3f pos =
                    transforms[i].position - (c.magnitude * c.direction);
                transforms[i].position = pos;
                colliders[i].Position() = pos;
            }
        }
    }
}

verna::Vec3f Snake::Direction() const {
    return transforms[0].Forward();
}

verna::Vec3f Snake::HeadPosition() const {
    return transforms[0].position;
}

bool Snake::Hits(const verna::BoundingSphere& sphere) const {
    return verna::SphereCollide(colliders.front(), sphere);
}

bool Snake::Hits(BufferView<verna::BoundingBox> boxes) const {
    verna::BoundingBox box;
    box.Recalculate(mesh, transforms.front());
    for (const auto& b : boxes)
        if (verna::BoxCollide(b, box))
            return true;
    return false;
}

void Snake::GrowTail() {
    verna::Transform t = transforms.back();
    auto direction = t.position - transforms[transforms.size() - 2].position;
    t.position = t.position + direction;
    transforms.push_back(t);
    colliders.push_back(verna::BoundingSphere());
    colliders.back().Recalculate(mesh, t);
}

bool Snake::HitsItself() const {
    for (size_t i = 2; i < colliders.size(); i++)
        if (Hits(colliders[i]))
            return true;
    return false;
}

}  // namespace snake