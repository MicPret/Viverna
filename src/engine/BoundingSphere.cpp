#include <viverna/core/BoundingSphere.hpp>
#include <viverna/core/BoundingBox.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Mesh.hpp>

namespace verna {
bool BoundingSphere::Collides(const BoundingBox& box) const {
    return box.Collides(*this);
}

void BoundingSphere::Recalculate(const Mesh& mesh, const Transform& transform) {
    position = transform.position;
    radius = 0.0f;
    if (mesh.vertices.empty()) {
        VERNA_LOGW("Called BoundingSphere::Recalculate on empty mesh!");
        return;
    }
    for (const Vertex& v : mesh.vertices) {
        float square_dist =
            transform.scale.ComponentProduct(v.position).SquaredMagnitude();
        if (square_dist > radius)
            radius = square_dist;
    }
    radius = maths::Sqrt(radius);
}

void BoundingSphere::Recalculate(const Mesh& mesh) {
    position = Vec3f();
    radius = 0.0f;
    if (mesh.vertices.empty()) {
        VERNA_LOGW("Called BoundingSphere::Recalculate on empty mesh!");
        return;
    }
    for (const Vertex& v : mesh.vertices) {
        float square_dist = v.position.SquaredMagnitude();
        if (square_dist > radius)
            radius = square_dist;
    }
    radius = maths::Sqrt(radius);
}
}  // namespace verna