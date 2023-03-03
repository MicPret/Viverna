#ifndef VERNA_POINT_LIGHT_HPP
#define VERNA_POINT_LIGHT_HPP

#include <viverna/maths/Vec3f.hpp>

namespace verna {
struct PointLight {
    // TODO calculate from radius
    struct Attenuation {
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    };

    Vec3f position;
    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;
    Attenuation attenuation;
};
}  // namespace verna

#endif