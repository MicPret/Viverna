#ifndef VERNA_GPU_DIRECTION_LIGHT_DATA_HPP
#define VERNA_GPU_DIRECTION_LIGHT_DATA_HPP

#include <viverna/graphics/DirectionLight.hpp>
#include <viverna/maths/Vec4f.hpp>

namespace verna::gpu {
struct DirectionLightData {
    Vec4f direction;
    Vec4f ambient;
    Vec4f diffuse;
    Vec4f specular;
    Mat4f pv_matrix;
    constexpr DirectionLightData() = default;
    constexpr DirectionLightData(const DirectionLight& light) :
        direction(light.direction, 0.0f),
        ambient(light.ambient, 0.0),
        diffuse(light.diffuse, 0.0f),
        specular(light.specular, 0.0f) {
        constexpr float near = 0.15f;
        constexpr float far = 100.0f;
        constexpr float delta = 32.0f;
        auto projection = Mat4f::Ortho(-delta, delta, -delta, delta, near, far);
        auto view = Mat4f::LookAt(-direction.Xyz(), Vec3f(), Vec3f::UnitY());
        pv_matrix = projection * view;
    }
};
}  // namespace verna::gpu

#endif
