#ifndef VERNA_COLLISION_HPP
#define VERNA_COLLISION_HPP

#include <viverna/core/BoundingBox.hpp>
#include <viverna/core/BoundingSphere.hpp>
#include <viverna/maths/MathUtils.hpp>

#include <array>

namespace verna {
struct Collision {
    Vec3f direction;
    float magnitude;
};

constexpr bool SphereCollide(const BoundingSphere& a, const BoundingSphere& b) {
    Vec3f a_to_b = b.Position() - a.Position();
    float squared_distance = a_to_b.SquaredMagnitude();
    float radius_sum = a.Radius() + b.Radius();
    return squared_distance <= (radius_sum * radius_sum);
}

constexpr bool SphereCollide(const BoundingSphere& a,
                             const BoundingSphere& b,
                             Collision& output) {
    Vec3f a_to_b = b.Position() - a.Position();
    float squared_distance = a_to_b.SquaredMagnitude();
    float radius_sum = a.Radius() + b.Radius();
    if (squared_distance > (radius_sum * radius_sum))
        return false;
    float distance = maths::Sqrt(squared_distance);
    output.magnitude = radius_sum - distance;
    output.direction = (1.0f / distance) * a_to_b;
    return true;
}

constexpr bool BoxCollide(const BoundingBox& a, const BoundingBox& b) {
    Vec3f b_a = b.MaxPosition() - a.MinPosition();
    if (b_a.x < 0.0f || b_a.y < 0.0f || b_a.z < 0.0f)
        return false;
    Vec3f a_b = a.MaxPosition() - b.MinPosition();
    return a_b.x >= 0.0 && a_b.y >= 0.0f && a_b.z >= 0.0f;
}

constexpr bool BoxCollide(const BoundingBox& a,
                          const BoundingBox& b,
                          Collision& output) {
    Vec3f b_a = b.MaxPosition() - a.MinPosition();
    Vec3f a_b = a.MaxPosition() - b.MinPosition();
    std::array<float, 6> d = {b_a.x, a_b.x, b_a.y, a_b.y, b_a.z, a_b.z};
    size_t index = 0;
    float inside = d[0];
    for (size_t i = 1; i < d.size(); i++) {
        if (d[i] < inside) {
            inside = d[i];
            index = i;
        }
    }
    if (inside < 0)
        return false;

    constexpr std::array<Vec3f, 6> dirs = {-Vec3f::UnitX(), Vec3f::UnitX(),
                                           -Vec3f::UnitY(), Vec3f::UnitY(),
                                           -Vec3f::UnitZ(), Vec3f::UnitZ()};
    output.direction = dirs[index];
    output.magnitude = inside;
    /*
    d[0] = output.direction.x;
    d[1] = output.direction.y;
    d[2] = output.direction.z;
    float proj = d[index / 2];
    output.magnitude = inside * inside / proj;
    */
    return true;
}

}  // namespace verna

#endif