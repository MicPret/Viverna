#ifndef VERNA_MAT3F_HPP
#define VERNA_MAT3F_HPP

#include "Vec3f.hpp"
#include "Mat4f.hpp"

#include <array>
#include <cstddef>

namespace verna {
struct Mat3f {
    std::array<float, 9> raw;

    constexpr Mat3f() : raw{} {}
    constexpr Mat3f(const std::array<float, 9>& matrix) : raw(matrix) {}
    constexpr Mat3f(const Mat4f& matrix) :
        raw({matrix[0], matrix[1], matrix[2], matrix[4], matrix[5], matrix[6],
             matrix[8], matrix[9], matrix[10]}) {}
    constexpr Mat3f(float diagonal) :
        raw{
            diagonal, 0.0f,     0.0f,      // first column
            0.0f,     diagonal, 0.0f,      // second
            0.0f,     0.0f,     diagonal,  // third
        } {}
    constexpr const float& operator[](size_t index) const { return raw[index]; }
    constexpr float& operator[](size_t index) { return raw[index]; }
    constexpr Mat3f Transposed() const;

    static constexpr Mat3f Identity() { return Mat3f(1.0f); }
    static Mat3f Rotation(const Vec3f& unit_axis, float radians);
};

/**
 * @brief Computes the matrix multiplication
 *
 * @param a left operand
 * @param b right operand
 * @return The matrix product
 */
constexpr Mat3f operator*(const Mat3f& a, const Mat3f& b) {
    Mat3f result;
    result[0] = a[0] * b[0] + a[3] * b[1] + a[6] * b[2];
    result[1] = a[1] * b[0] + a[4] * b[1] + a[7] * b[2];
    result[2] = a[2] * b[0] + a[5] * b[1] + a[8] * b[2];
    result[3] = a[0] * b[3] + a[3] * b[4] + a[6] * b[5];
    result[4] = a[1] * b[3] + a[4] * b[4] + a[7] * b[5];
    result[5] = a[2] * b[3] + a[5] * b[4] + a[8] * b[5];
    result[6] = a[0] * b[6] + a[3] * b[7] + a[6] * b[8];
    result[7] = a[1] * b[6] + a[4] * b[7] + a[7] * b[8];
    result[8] = a[2] * b[6] + a[5] * b[7] + a[8] * b[8];
    return result;
}

constexpr Vec3f operator*(const Mat3f& mtx, const Vec3f& vec) {
    return Vec3f(mtx[0] * vec.x + mtx[3] * vec.y + mtx[6] * vec.z,
                 mtx[1] * vec.x + mtx[4] * vec.y + mtx[7] * vec.z,
                 mtx[2] * vec.x + mtx[5] * vec.y + mtx[8] * vec.z);
}

constexpr Mat3f Mat3f::Transposed() const {
    return Mat3f({
        raw[0], raw[3], raw[6],  // first column
        raw[1], raw[4], raw[7],  // second
        raw[2], raw[5], raw[8],  // third
    });
}
}  // namespace verna

#endif