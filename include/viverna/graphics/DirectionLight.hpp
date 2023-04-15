#ifndef VERNA_DIRECTION_LIGHT_HPP
#define VERNA_DIRECTION_LIGHT_HPP

#include <viverna/maths/Vec3f.hpp>

namespace verna {
struct DirectionLight {
    Vec3f direction;
    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;
    constexpr DirectionLight() :
        direction(-Vec3f::UnitY()),
        ambient(0.05f),
        diffuse(0.8f),
        specular(0.3f) {}
};
}  // namespace verna

#endif