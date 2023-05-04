#include <viverna/core/Transform.hpp>

#include <cmath>

namespace verna {
void Transform::LookAt(const Vec3f& target) {
    Vec3f forward = Forward();
    Vec3f to_target = (target - position).Normalized();
    float cosangle = forward.Dot(to_target);
    if (cosangle < 0.99999f) {
        float angle =
            cosangle <= -1.0f ? verna::maths::Pi() : std::acos(cosangle);
        rotation =
            Quaternion(forward.Cross(to_target).Normalized(), angle) * rotation;
    }
}
}  // namespace verna