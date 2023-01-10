#ifndef VERNA_MAT4F_HPP
#define VERNA_MAT4F_HPP

#include "Vec3f.hpp"

#include <array>
#include <cstddef>

namespace verna {
struct Mat4f {
    std::array<float, 16> raw;

    constexpr Mat4f() : raw{} {}
    constexpr Mat4f(std::array<float, 16> matrix) : raw(matrix) {}
    constexpr Mat4f(float diagonal) :
        raw{
            diagonal, 0.0f,     0.0f,     0.0f,     // first
            0.0f,     diagonal, 0.0f,     0.0f,     // second
            0.0f,     0.0f,     diagonal, 0.0f,     // third
            0.0f,     0.0f,     0.0f,     diagonal  // fourth
        } {}
    constexpr const float& operator[](size_t index) const { return raw[index]; }
    constexpr float& operator[](size_t index) { return raw[index]; }

    static constexpr Mat4f Identity() { return Mat4f(1.0f); }
    static Mat4f Rotation(const Vec3f& unit_axis, float radians);
    static Mat4f Perspective(float fovy, float aspect, float near, float far);
    static constexpr Mat4f LookAt(const Vec3f& eye,
                                  const Vec3f& right,
                                  const Vec3f& up,
                                  const Vec3f& forward) {
        Vec3f e = -eye;
        Mat4f mat = Mat4f::Identity();
        mat[0] = right.x;
        mat[1] = up.x;
        mat[2] = forward.x;
        mat[4] = right.y;
        mat[5] = up.y;
        mat[6] = forward.y;
        mat[8] = right.z;
        mat[9] = up.z;
        mat[10] = forward.z;
        mat[12] = right.Dot(e);
        mat[13] = up.Dot(e);
        mat[14] = forward.Dot(e);
        return mat;
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
}  // namespace verna

#endif