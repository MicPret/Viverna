#include <viverna/core/BoundingBox.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Mesh.hpp>

namespace verna {
void BoundingBox::Recalculate(const std::vector<Vec3f>& world_coords) {
    Vec3f min = world_coords[0];
    Vec3f max = min;
    for (size_t i = 1; i < world_coords.size(); i++) {
        const Vec3f& v = world_coords[i];
        if (v.x <= min.x)
            min.x = v.x;
        else if (v.x > max.x)
            max.x = v.x;
        if (v.y <= min.y)
            min.y = v.y;
        else if (v.y > max.y)
            max.y = v.y;
        if (v.z <= min.z)
            min.z = v.z;
        else if (v.z > max.z)
            max.z = v.z;
    }
    new (this) BoundingBox(min, max - min);
}

void BoundingBox::Recalculate(const Mesh& mesh, const Transform& transform) {
    const size_t n = mesh.vertices.size();
    std::vector<Vec3f> world_coords(n);
    for (size_t i = 0; i < n; i++)
        world_coords[i] = transform.Apply(mesh.vertices[i].position);
    Recalculate(world_coords);
}
}  // namespace verna