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

Quaternion::Quaternion(const Mat4f& rot_matrix) {
    const Mat4f& r = rot_matrix;  // just an alias
    float t;
    Vec4f vec;
    if (r[10] < 0.0f) {
        if (r[0] > r[5]) {
            t = 1.0f + r[0] - r[5] - r[10];
            vec.x = t;
            vec.y = r[1] + r[4];
            vec.z = r[8] + r[2];
            vec.w = r[6] - r[9];
        } else {
            t = 1.0f - r[0] + r[5] - r[10];
            vec.x = r[1] + r[4];
            vec.y = t;
            vec.z = r[6] + r[9];
            vec.w = r[8] - r[2];
        }
    } else {
        if (r[0] < -r[5]) {
            t = 1.0f - r[0] - r[5] + r[10];
            vec.x = r[8] + r[2];
            vec.y = r[6] + r[9];
            vec.z = t;
            vec.w = r[1] - r[4];
        } else {
            t = 1.0f + r[0] + r[5] + r[10];
            vec.x = r[6] - r[9];
            vec.y = r[8] - r[2];
            vec.z = r[1] - r[4];
            vec.w = t;
        }
    }
    float scalar = 0.5f / maths::Sqrt(t);
    vec = scalar * vec;
    x = vec.x;
    y = vec.y;
    z = vec.z;
    w = vec.w;
}
}  // namespace verna