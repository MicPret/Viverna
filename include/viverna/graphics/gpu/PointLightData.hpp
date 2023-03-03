#ifndef VERNA_GPU_POINT_LIGHT_DATA_HPP
#define VERNA_GPU_POINT_LIGHT_DATA_HPP

#include <viverna/graphics/PointLight.hpp>
#include <viverna/maths/Vec4f.hpp>

namespace verna::gpu {
struct PointLightData {
    Vec4f position;
    Vec4f ambient_constant;
    Vec4f diffuse_linear;
    Vec4f specular_quadratic;
    constexpr PointLightData() = default;
    constexpr PointLightData(const PointLight& pl) :
        position(pl.position, 0.0f),
        ambient_constant(pl.ambient, pl.attenuation.constant),
        diffuse_linear(pl.diffuse, pl.attenuation.linear),
        specular_quadratic(pl.specular, pl.attenuation.quadratic) {}
};

}  // namespace verna::gpu

#endif