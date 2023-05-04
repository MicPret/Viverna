#include <viverna/core/Transform.hpp>

#include <cmath>

namespace verna {
void Transform::LookAt(const Vec3f& target) {
    constexpr float epsilon = 1e-5;
    constexpr float one_minus_epsilon = 1.0f - epsilon;
    Vec3f forward = Forward();
    Vec3f to_target = target - position;
    float mag = to_target.Magnitude();
    if (mag < epsilon)
        return;
    to_target = (1.0f / mag) * to_target;
    float cosangle = forward.Dot(to_target);
    if (cosangle >= one_minus_epsilon)
        return;
    float angle = cosangle <= -one_minus_epsilon ? verna::maths::Pi()
                                                 : std::acos(cosangle);
    rotation =
        Quaternion(forward.Cross(to_target).Normalized(), angle) * rotation;
}
}  // namespace verna