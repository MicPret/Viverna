#include <viverna/graphics/gpu/PointLightData.hpp>

namespace verna::gpu {

static constexpr void Attenuation(float radius,
                                  float& constant,
                                  float& linear,
                                  float& quadratic) {
    // TODO from radius!
    constant = 1.0f;
    linear = 0.09f;
    quadratic = 0.032f;
}

PointLightData::PointLightData(const PointLight& pl) :
    position(pl.position, 0.0f),
    ambient_constant(pl.ambient, 0.0),
    diffuse_linear(pl.diffuse, 0.0),
    specular_quadratic(pl.specular, 0.0) {
    Attenuation(pl.radius, ambient_constant.w, diffuse_linear.w,
                specular_quadratic.w);
    // TODO check!
    constexpr std::array directions = {Vec3f::UnitX(), -Vec3f::UnitX(),
                                       Vec3f::UnitY(), -Vec3f::UnitY(),
                                       Vec3f::UnitZ(), -Vec3f::UnitZ()};

    constexpr std::array ups = {Vec3f::UnitY(), Vec3f::UnitY(), -Vec3f::UnitZ(),
                                Vec3f::UnitZ(), Vec3f::UnitY(), Vec3f::UnitY()};
    for (size_t i = 0; i < 6; i++)
        view_matrices[i] =
            Mat4f::LookAt(pl.position, pl.position + directions[i], ups[i]);
}
}  // namespace verna::gpu
