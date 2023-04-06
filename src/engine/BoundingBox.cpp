#include <viverna/core/BoundingBox.hpp>
#include <viverna/core/BoundingSphere.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Mesh.hpp>

namespace verna {
bool BoundingBox::Collides(const BoundingSphere& sphere) const {
    float dist_squared = sphere.Radius() * sphere.Radius();
    Vec3f s = sphere.Position();
    Vec3f min = MinPosition();
    Vec3f max = MaxPosition();
    if (s.x < min.x)
        dist_squared -= maths::Squared(s.x - min.x);
    else if (s.x > max.x)
        dist_squared -= maths::Squared(s.x - max.x);
    if (s.y < min.y)
        dist_squared -= maths::Squared(s.y - min.y);
    else if (s.y > max.y)
        dist_squared -= maths::Squared(s.y - max.y);
    if (s.z < min.z)
        dist_squared -= maths::Squared(s.z - min.z);
    else if (s.z > max.z)
        dist_squared -= maths::Squared(s.z - max.z);
    return dist_squared > 0;
}

void BoundingBox::ApplyTransform(const Transform& transform) {
    std::array vertices = Vertices();
    for (Vec3f& v : vertices)
        v = transform.Apply(v);
    Recalculate(vertices);
}

void BoundingBox::Recalculate(const Mesh& mesh, const Transform& transform) {
    if (mesh.vertices.empty()) {
        VERNA_LOGW("Called BoundingBox::Recalculate on empty mesh!");
        position = Vec3f();
        size = Vec3f();
        return;
    }
    Vec3f pos = transform.Apply(mesh.vertices[0].position);
    Vec3f min = pos;
    Vec3f max = pos;
    for (size_t i = 1; i < mesh.vertices.size(); i++) {
        pos = transform.Apply(mesh.vertices[i].position);
        min = Vec3f::Min(min, pos);
        max = Vec3f::Max(max, pos);
    }
    position = min;
    size = max - min;
}

void BoundingBox::Recalculate(const Mesh& mesh) {
    if (mesh.vertices.empty()) {
        VERNA_LOGW("Called BoundingBox::Recalculate on empty mesh!");
        position = Vec3f();
        size = Vec3f();
        return;
    }
    Vec3f min = mesh.vertices[0].position;
    Vec3f max = min;
    for (size_t i = 1; i < mesh.vertices.size(); i++) {
        const Vec3f& pos = mesh.vertices[i].position;
        min = Vec3f::Min(min, pos);
        max = Vec3f::Max(max, pos);
    }
    position = min;
    size = max - min;
}
}  // namespace verna