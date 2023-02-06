#ifndef VERNA_BOUNDING_SPHERE_HPP
#define VERNA_BOUNDING_SPHERE_HPP

#include <viverna/maths/Vec3f.hpp>

#include <vector>

namespace verna {

struct Mesh;
struct Transform;

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
    void Recalculate(const Mesh& mesh, const Transform& transform);

   private:
    Vec3f position;
    float radius;
};
}  // namespace verna

#endif