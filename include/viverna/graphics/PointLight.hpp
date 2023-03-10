#ifndef VERNA_POINT_LIGHT_HPP
#define VERNA_POINT_LIGHT_HPP

#include <viverna/maths/Vec3f.hpp>

namespace verna {
struct PointLight {
    Vec3f position;
    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;
    float radius;
};
}  // namespace verna

#endif