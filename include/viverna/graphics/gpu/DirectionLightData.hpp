#ifndef VERNA_GPU_DIRECTION_LIGHT_DATA_HPP
#define VERNA_GPU_DIRECTION_LIGHT_DATA_HPP

#include <viverna/graphics/DirectionLight.hpp>
#include <viverna/maths/Mat4f.hpp>
#include <viverna/maths/Vec4f.hpp>

namespace verna::gpu {
struct DirectionLightData {
    Vec4f direction;
    Vec4f ambient_dirty;
    Vec4f diffuse;
    Vec4f specular;
    Mat4f pv_matrix;
    constexpr DirectionLightData() = default;
};
}  // namespace verna::gpu

#endif
