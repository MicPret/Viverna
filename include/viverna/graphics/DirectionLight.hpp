#ifndef VERNA_DIRECTION_LIGHT_HPP
#define VERNA_DIRECTION_LIGHT_HPP

#include <viverna/maths/Vec3f.hpp>

namespace verna {
struct DirectionLight {
    Vec3f direction;
    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;
};
}  // namespace verna

#endif