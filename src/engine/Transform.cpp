#include <viverna/core/Transform.hpp>

namespace verna {
void Transform::LookAt(const Vec3f& pos) {
    Mat4f rot = Mat4f::LookAt(position, position + Forward(), Vec3f::UnitY());
    rotation = Quaternion(rot);
}
}  // namespace verna