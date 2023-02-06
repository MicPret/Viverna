#ifndef VERNA_MATH_DEFINES_HPP
#define VERNA_MATH_DEFINES_HPP

#include <cstddef>
#include <limits>

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
 * @brief Computes the max value between x, y and z
 *
 * @param x First parameter
 * @param y Second parameter
 * @param z Third parameter
 * @return Max value between three floats
 */
constexpr float Max(float x, float y, float z) {
    return Max(Max(x, y), z);
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
constexpr size_t NextPOT(size_t x) {
    size_t n = x - 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

/**
 * @brief Computes the smallest float greater than x
 *
 * @param x Input value
 * @return The smallest float greater than x
 */
float NextFloat(float x);

/**
 * @brief Generates a random floating-point number between 0 and 1, inclusive
 *
 * @return A random float in the interval [0, 1]
 */
float RandomFloatInclusive();

/**
 * @brief Generates a random floating-point number between 0 and 1, exclusive
 *
 * @return A random float in the interval [0, 1)
 */
float RandomFloatExclusive();

}  // namespace verna::maths

#endif