#ifndef VERNA_MATH_DEFINES_HPP
#define VERNA_MATH_DEFINES_HPP

#include <limits>

namespace verna::maths {
namespace detail {
constexpr float NewtonRaphson(float x, float curr, float prev) {
    return curr == prev ? curr
                        : NewtonRaphson(x, 0.5 * (curr + x / curr), curr);
}
}  // namespace detail

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
}  // namespace verna::maths

#endif