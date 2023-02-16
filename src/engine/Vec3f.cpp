#include <viverna/maths/Vec3f.hpp>
#include <viverna/maths/MathUtils.hpp>

#include <cmath>

namespace verna {

Vec3f Vec3f::FromPolarCoordinates(float azimuth, float zenit, float magnitude) {
    float sin_phi = sin(zenit);

    float x = sin_phi * sin(azimuth);
    float y = cos(zenit);
    float z = sin_phi * cos(azimuth);

    return magnitude * Vec3f(x, y, z);
}

Vec3f Vec3f::Random() {
    float azimuth_angle = maths::RandomFloatExclusive() * (2.0f * maths::Pi());
    float zenit_angle = maths::RandomFloatExclusive() * maths::Pi();

    return FromPolarCoordinates(azimuth_angle, zenit_angle);
}
}  // namespace verna