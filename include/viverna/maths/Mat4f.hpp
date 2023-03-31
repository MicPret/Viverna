#ifndef VERNA_MAT4F_HPP
#define VERNA_MAT4F_HPP

#include "Vec3f.hpp"
#include "Vec4f.hpp"
#include <viverna/core/Debug.hpp>

#include <array>
#include <cstddef>

namespace verna {
struct Mat4f {
    std::array<float, 16> raw;

    constexpr Mat4f() : raw{} {}
    constexpr Mat4f(const std::array<float, 16>& matrix) : raw(matrix) {}
    constexpr Mat4f(float diagonal) :
        raw{
            diagonal, 0.0f,     0.0f,     0.0f,     // first column
            0.0f,     diagonal, 0.0f,     0.0f,     // second
            0.0f,     0.0f,     diagonal, 0.0f,     // third
            0.0f,     0.0f,     0.0f,     diagonal  // fourth
        } {}
    constexpr const float& operator[](size_t index) const { return raw[index]; }
    constexpr float& operator[](size_t index) { return raw[index]; }
    constexpr Mat4f Inverted() const;
    constexpr Mat4f Transposed() const;

    static constexpr Mat4f Identity() { return Mat4f(1.0f); }
    static Mat4f Rotation(const Vec3f& unit_axis, float radians);
    static Mat4f Perspective(float fovy, float aspect, float near, float far);
    static constexpr Mat4f Ortho(float left,
                                 float right,
                                 float top,
                                 float bottom,
                                 float near,
                                 float far) {
        float i_dx = 1.0f / (right - left);
        float i_dy = 1.0f / (top - bottom);
        float i_dz = 1.0f / (far - near);
        Mat4f result;
        result[0] = 2.0f * i_dx;
        result[5] = 2.0f * i_dy;
        result[10] = i_dz;
        result[12] = -(right + left) * i_dx;
        result[13] = -(top + bottom) * i_dy;
        result[14] = -near * i_dz;
        result[15] = 1.0f;
        return result;
    }
    static constexpr Mat4f LookAt(const Vec3f& eye,
                                  const Vec3f& target,
                                  const Vec3f& up) {
        Vec3f forward = (target - eye).Normalized();
        Vec3f right = up.Cross(forward).Normalized();
        Vec3f u = forward.Cross(right);

        Mat4f result;
        result[0] = right.x;  // first column
        result[1] = u.x;
        result[2] = forward.x;
        result[3] = 0.0f;
        result[4] = right.y;  // second column
        result[5] = u.y;
        result[6] = forward.y;
        result[7] = 0.0f;
        result[8] = right.z;  // third column
        result[9] = u.z;
        result[10] = forward.z;
        result[11] = 0.0f;
        result[12] = -right.Dot(eye);  // fourth column
        result[13] = -u.Dot(eye);
        result[14] = -forward.Dot(eye);
        result[15] = 1.0f;
        return result;
    }
};

/**
 * @brief Computes the matrix multiplication
 *
 * @param a left operand
 * @param b right operand
 * @return The matrix product
 */
constexpr Mat4f operator*(const Mat4f& a, const Mat4f& b) {
    Mat4f result;
    result[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
    result[1] = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
    result[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
    result[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];
    result[4] = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
    result[5] = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
    result[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
    result[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];
    result[8] = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
    result[9] = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
    result[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
    result[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];
    result[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
    result[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
    result[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
    result[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
    return result;
}

constexpr Vec4f operator*(const Mat4f& mtx, const Vec4f& vec) {
    return Vec4f(
        mtx[0] * vec.x + mtx[4] * vec.y + mtx[8] * vec.z + mtx[12] * vec.w,
        mtx[1] * vec.x + mtx[5] * vec.y + mtx[9] * vec.z + mtx[13] * vec.w,
        mtx[2] * vec.x + mtx[6] * vec.y + mtx[10] * vec.z + mtx[14] * vec.w,
        mtx[3] * vec.x + mtx[7] * vec.y + mtx[11] * vec.z + mtx[15] * vec.w);
}

constexpr Mat4f Mat4f::Inverted() const {
    Mat4f inv;
    inv[0] = raw[5] * raw[10] * raw[15] - raw[5] * raw[11] * raw[14]
             - raw[9] * raw[6] * raw[15] + raw[9] * raw[7] * raw[14]
             + raw[13] * raw[6] * raw[11] - raw[13] * raw[7] * raw[10];

    inv[4] = -raw[4] * raw[10] * raw[15] + raw[4] * raw[11] * raw[14]
             + raw[8] * raw[6] * raw[15] - raw[8] * raw[7] * raw[14]
             - raw[12] * raw[6] * raw[11] + raw[12] * raw[7] * raw[10];

    inv[8] = raw[4] * raw[9] * raw[15] - raw[4] * raw[11] * raw[13]
             - raw[8] * raw[5] * raw[15] + raw[8] * raw[7] * raw[13]
             + raw[12] * raw[5] * raw[11] - raw[12] * raw[7] * raw[9];

    inv[12] = -raw[4] * raw[9] * raw[14] + raw[4] * raw[10] * raw[13]
              + raw[8] * raw[5] * raw[14] - raw[8] * raw[6] * raw[13]
              - raw[12] * raw[5] * raw[10] + raw[12] * raw[6] * raw[9];

    inv[1] = -raw[1] * raw[10] * raw[15] + raw[1] * raw[11] * raw[14]
             + raw[9] * raw[2] * raw[15] - raw[9] * raw[3] * raw[14]
             - raw[13] * raw[2] * raw[11] + raw[13] * raw[3] * raw[10];

    inv[5] = raw[0] * raw[10] * raw[15] - raw[0] * raw[11] * raw[14]
             - raw[8] * raw[2] * raw[15] + raw[8] * raw[3] * raw[14]
             + raw[12] * raw[2] * raw[11] - raw[12] * raw[3] * raw[10];

    inv[9] = -raw[0] * raw[9] * raw[15] + raw[0] * raw[11] * raw[13]
             + raw[8] * raw[1] * raw[15] - raw[8] * raw[3] * raw[13]
             - raw[12] * raw[1] * raw[11] + raw[12] * raw[3] * raw[9];

    inv[13] = raw[0] * raw[9] * raw[14] - raw[0] * raw[10] * raw[13]
              - raw[8] * raw[1] * raw[14] + raw[8] * raw[2] * raw[13]
              + raw[12] * raw[1] * raw[10] - raw[12] * raw[2] * raw[9];

    inv[2] = raw[1] * raw[6] * raw[15] - raw[1] * raw[7] * raw[14]
             - raw[5] * raw[2] * raw[15] + raw[5] * raw[3] * raw[14]
             + raw[13] * raw[2] * raw[7] - raw[13] * raw[3] * raw[6];

    inv[6] = -raw[0] * raw[6] * raw[15] + raw[0] * raw[7] * raw[14]
             + raw[4] * raw[2] * raw[15] - raw[4] * raw[3] * raw[14]
             - raw[12] * raw[2] * raw[7] + raw[12] * raw[3] * raw[6];

    inv[10] = raw[0] * raw[5] * raw[15] - raw[0] * raw[7] * raw[13]
              - raw[4] * raw[1] * raw[15] + raw[4] * raw[3] * raw[13]
              + raw[12] * raw[1] * raw[7] - raw[12] * raw[3] * raw[5];

    inv[14] = -raw[0] * raw[5] * raw[14] + raw[0] * raw[6] * raw[13]
              + raw[4] * raw[1] * raw[14] - raw[4] * raw[2] * raw[13]
              - raw[12] * raw[1] * raw[6] + raw[12] * raw[2] * raw[5];

    inv[3] = -raw[1] * raw[6] * raw[11] + raw[1] * raw[7] * raw[10]
             + raw[5] * raw[2] * raw[11] - raw[5] * raw[3] * raw[10]
             - raw[9] * raw[2] * raw[7] + raw[9] * raw[3] * raw[6];

    inv[7] = raw[0] * raw[6] * raw[11] - raw[0] * raw[7] * raw[10]
             - raw[4] * raw[2] * raw[11] + raw[4] * raw[3] * raw[10]
             + raw[8] * raw[2] * raw[7] - raw[8] * raw[3] * raw[6];

    inv[11] = -raw[0] * raw[5] * raw[11] + raw[0] * raw[7] * raw[9]
              + raw[4] * raw[1] * raw[11] - raw[4] * raw[3] * raw[9]
              - raw[8] * raw[1] * raw[7] + raw[8] * raw[3] * raw[5];

    inv[15] = raw[0] * raw[5] * raw[10] - raw[0] * raw[6] * raw[9]
              - raw[4] * raw[1] * raw[10] + raw[4] * raw[2] * raw[9]
              + raw[8] * raw[1] * raw[6] - raw[8] * raw[2] * raw[5];

    float det =
        raw[0] * inv[0] + raw[1] * inv[4] + raw[2] * inv[8] + raw[3] * inv[12];

    [[maybe_unused]] auto ToString = [](const std::array<float, 16>& arr) {
        std::string result;
        for (size_t i = 0; i < 4; i++) {
            result += std::to_string(arr[i]) + '\t' + std::to_string(arr[i + 4])
                      + '\t' + std::to_string(arr[i + 8]) + '\t'
                      + std::to_string(arr[i + 12]) + '\n';
        }
        return result;
    };
    VERNA_LOGE_IF(det == 0.0f,
                  "Math error: can't invert matrix!\n" + ToString(raw));

    det = 1.0 / det;
    for (auto i = 0; i < 16; i++)
        inv[i] *= det;
    return inv;
}

constexpr Mat4f Mat4f::Transposed() const {
    return Mat4f({
        raw[0], raw[4], raw[8], raw[12],   // first column
        raw[1], raw[5], raw[9], raw[13],   // second
        raw[2], raw[6], raw[10], raw[14],  // third
        raw[3], raw[7], raw[11], raw[15],  // fourth
    });
}
}  // namespace verna

#endif