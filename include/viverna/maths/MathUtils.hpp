#ifndef VERNA_MATH_DEFINES_HPP
#define VERNA_MATH_DEFINES_HPP

#include <limits>
#include <cstdint>

namespace verna::maths {
namespace detail {
constexpr float NewtonRaphson(float x, float curr, float prev) {
    return curr == prev ? curr
                        : NewtonRaphson(x, 0.5 * (curr + x / curr), curr);
}
}  // namespace detail

/**
 * @brief Value of pi, equal to a 180° angle in radians
 *
 * @return pi as a float
 */
constexpr float Pi() {
    return 3.1415926535f;
}

/**
 * @brief Returns true for floats whose distance is less than epsilon
 *
 * @param a First float
 * @param b Second float
 * @param epsilon Max allowed distance (exclusive)
 * @return true for close floats
 */
constexpr bool AreAlmostEqual(float a, float b, float epsilon) {
    float x = a - b;
    return (x > -epsilon) && (x < epsilon);
}

/**
 * @brief Returns the square root for non-negative finite x, otherwise NaN
 *
 * @param x Input float
 * @return The square root
 */
constexpr float Sqrt(float x) {
    return x >= 0 && x < std::numeric_limits<float>::infinity()
               ? detail::NewtonRaphson(x, x, 0)
               : std::numeric_limits<float>::quiet_NaN();
}

/**
 * @brief Converts an angle from degrees to radians
 *
 * @param degrees Angle in degrees
 * @return Same angle in radians
 */
constexpr float Radians(float degrees) {
    return degrees * (Pi() / 180.0f);
}

/**
 * @brief Converts an angle from radians to degrees
 *
 * @param degrees Angle in radians
 * @return Same angle in degrees
 */
constexpr float Degrees(float radians) {
    return radians * (180.0f / Pi());
}

/**
 * @brief Performs linear interpolation between x and y
 *
 * @param x First parameter
 * @param y Second parameter
 * @param t Scalar between 0 and 1
 * @return constexpr float
 */
constexpr float Lerp(float x, float y, float t) {
    return x * (1.0f - t) + y * t;
}

/**
 * @brief Computes the max value between x and y
 *
 * @param x First parameter
 * @param y Second parameter
 * @return Max value between two floats
 */
constexpr float Max(float x, float y) {
    return x >= y ? x : y;
}

/**
 * @brief Computes the min value between x and y
 *
 * @param x First parameter
 * @param y Second parameter
 * @return Min value between two floats
 */
constexpr float Min(float x, float y) {
    return x <= y ? x : y;
}

/**
 * @brief Computes the max value between x and y
 *
 * @param x First parameter
 * @param y Second parameter
 * @return Max value between two ints
 */
constexpr int Max(int x, int y) {
    return x >= y ? x : y;
}

/**
 * @brief Computes the min value between x and y
 *
 * @param x First parameter
 * @param y Second parameter
 * @return Min value between two ints
 */
constexpr int Min(int x, int y) {
    return x <= y ? x : y;
}

/**
 * @brief Returns the next Power Of Two, or x itself if it's a POT
 *
 * @param x Input value
 * @return Next Power Of Two, or x itself if it's a POT
 */
constexpr uint32_t NextPOT(uint32_t x) {
    uint32_t n = x - 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

}  // namespace verna::maths

#endif