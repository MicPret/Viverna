#ifndef VERNA_GPU_POINT_LIGHT_DATA_HPP
#define VERNA_GPU_POINT_LIGHT_DATA_HPP

#include <viverna/graphics/PointLight.hpp>
#include <viverna/maths/Vec4f.hpp>
#include <viverna/maths/Mat4f.hpp>

namespace verna::gpu {
struct PointLightData {
    Vec4f position;
    Vec4f ambient_constant;
    Vec4f diffuse_linear;
    Vec4f specular_quadratic;
    std::array<Mat4f, 6> view_matrices;
    constexpr PointLightData() = default;
    PointLightData(const PointLight& pl);
};

}  // namespace verna::gpu

#endif