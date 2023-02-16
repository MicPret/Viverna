#include <viverna/physics/Collision.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Mesh.hpp>

namespace verna {
float BoundingSphereRadius(const Mesh& mesh) {
    float d = 0.0f;
    for (const Vertex& v : mesh.vertices) {
        float sm = v.position.SquaredMagnitude();
        if (sm > d)
            d = sm;
    }
    return maths::Sqrt(d);
}
}  // namespace verna