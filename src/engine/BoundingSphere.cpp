#include <viverna/core/BoundingSphere.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Mesh.hpp>

namespace verna {
void BoundingSphere::Recalculate(const Mesh& mesh, const Transform& transform) {
    position = transform.position;
    radius = 0.0f;
    for (const auto& v : mesh.vertices) {
        float d2 = Vec3f(transform.scale.x * v.position.x,
                         transform.scale.y * v.position.y,
                         transform.scale.z * v.position.z)
                       .SquaredMagnitude();
        if (d2 > radius)
            radius = d2;
    }
    radius = maths::Sqrt(radius);
}
}  // namespace verna