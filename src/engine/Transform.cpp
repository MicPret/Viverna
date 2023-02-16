#include <viverna/core/Transform.hpp>

namespace verna {

Vec3f Transform::Forward() const {
    return rotation.Rotate(Vec3f::UnitZ()).Normalized();
}

Vec3f Transform::Right() const {
    return Vec3f::UnitY().Cross(Forward()).Normalized();
}

Vec3f Transform::Up() const {
    return Forward().Cross(Right()).Normalized();
}
}  // namespace verna