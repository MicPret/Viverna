#include <viverna/core/Transform.hpp>

#include <cmath>

namespace verna {
void Transform::LookAt(const Vec3f& pos, const Vec3f& up) {
    Vec3f forward = up.x < 0.99999f ? Vec3f::UnitX().Cross(up).Normalized()
                                    : Vec3f::UnitY();
    Vec3f to_pos = (pos - position).Normalized();
    Vec3f axis = forward.Cross(to_pos);
    float cosangle = forward.Dot(to_pos);
    if (cosangle < 0.99999f) {
        float angle =
            cosangle <= -1.0f ? verna::maths::Pi() : std::acos(cosangle);
        rotation = Quaternion(axis.Normalized(), angle);
    }
}
}  // namespace verna