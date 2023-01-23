#include <viverna/maths/Mat4f.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/maths/Vec3f.hpp>

#include <cmath>

namespace verna {
Mat4f Mat4f::Rotation(const Vec3f& unit_axis, float radians) {
    VERNA_ASSERT(unit_axis.IsUnitVector());
    float cosine = std::cos(radians);
    float comp_cos = 1.0f - cosine;
    float sine = std::sin(radians);

    std::array<float, 16> matrix = {
        cosine + unit_axis.x * unit_axis.x * comp_cos,  // first
        unit_axis.x * unit_axis.y * comp_cos + unit_axis.z * sine,
        unit_axis.x * unit_axis.z * comp_cos - unit_axis.y * sine,
        0.0f,
        unit_axis.x * unit_axis.y * comp_cos - unit_axis.z * sine,  // second
        cosine + unit_axis.y * unit_axis.y * comp_cos,
        unit_axis.y * unit_axis.z * comp_cos + unit_axis.x * sine,
        0.0f,
        unit_axis.x * unit_axis.z * comp_cos + unit_axis.y * sine,  // third
        unit_axis.y * unit_axis.z * comp_cos - unit_axis.x * sine,
        cosine + unit_axis.z * unit_axis.z * comp_cos,
        0.0f,
        0.0f,  // fourth
        0.0f,
        0.0f,
        1.0f};
    return Mat4f(matrix);
}

Mat4f Mat4f::Perspective(float fovy, float aspect, float near, float far) {
    const float tanHalfFovy = std::tan(fovy * 0.5f);
    const float inverse_depth = 1.0f / (far - near);

    Mat4f result;
    result[0] = 1.0f / (aspect * tanHalfFovy);
    result[5] = 1.0f / tanHalfFovy;
    result[10] = -(far + near) * inverse_depth;
    result[11] = -1.0f;
    result[14] = -(2.0f * far * near) * inverse_depth;
    return result;
}
}  // namespace verna