#ifndef VERNA_MAT4F_HPP
#define VERNA_MAT4F_HPP

#include <array>
#include <cstddef>

namespace verna {

struct Mat4f {
    static constexpr size_t NUM_ROWS = 4;
    static constexpr size_t NUM_COLUMNS = NUM_ROWS;

    constexpr Mat4f() : raw{} {}
    constexpr Mat4f(std::array<float, NUM_ROWS * NUM_COLUMNS> matrix)
        : raw(matrix) {}
    std::array<float, NUM_ROWS * NUM_COLUMNS> raw;

    static constexpr Mat4f Identity() {
        return Mat4f({1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f});
    }
};

}  // namespace verna

#endif