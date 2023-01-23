#include <viverna/maths/Quaternion.hpp>
#include <viverna/core/Debug.hpp>

#include <cmath>

namespace verna {
Quaternion::Quaternion(const Vec3f& unit_axis, float radians) {
    VERNA_LOGW_IF(!unit_axis.IsUnitVector(),
                  "Quaternion constructed from non-unit vector as axis!");

    float half_radians = radians * 0.5f;
    float sine = std::sin(half_radians);
    float cosine = std::cos(half_radians);

    x = unit_axis.x * sine;
    y = unit_axis.y * sine;
    z = unit_axis.z * sine;
    w = cosine;
}
}  // namespace verna