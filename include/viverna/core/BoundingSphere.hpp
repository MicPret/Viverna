#ifndef VERNA_BOUNDING_SPHERE_HPP
#define VERNA_BOUNDING_SPHERE_HPP

#include <viverna/maths/Vec3f.hpp>

#include <vector>

namespace verna {

struct Mesh;
struct Transform;
class BoundingBox;

/**
 * @brief Bounding sphere in world coordinates
 *
 */
class BoundingSphere {
   public:
    constexpr BoundingSphere() : radius(0.0f) {}
    constexpr BoundingSphere(const Vec3f& position_, float radius_) :
        position(position_), radius(radius_) {}
    constexpr const Vec3f& Position() const { return position; }
    constexpr Vec3f& Position() { return position; }
    constexpr float Radius() const { return radius; }
    constexpr float& Radius() { return radius; }
    constexpr bool Contains(const Vec3f& point) const {
        return (point - position).SquaredMagnitude() <= maths::Squared(radius);
    }
    constexpr bool Collides(const BoundingSphere& other) const {
        Vec3f a_to_b = other.position - position;
        float radius_sum = radius + other.radius;
        return a_to_b.SquaredMagnitude() <= maths::Squared(radius_sum);
    }
    bool Collides(const BoundingBox& box) const;
    void Recalculate(const Mesh& mesh, const Transform& transform);
    void Recalculate(const Mesh& mesh);

   private:
    Vec3f position;
    float radius;
};
}  // namespace verna

#endif