#ifndef VERNA_MATERIAL_HPP
#define VERNA_MATERIAL_HPP

#include <array>
#include "Texture.hpp"

namespace verna {
struct Material {
    std::array<TextureId, 8> textures = {};
    std::array<float, 4> parameters = {};

    static constexpr size_t DIFFUSE_INDEX = 0;
    static constexpr size_t SPECULAR_INDEX = 1;
    static constexpr size_t SHININESS_INDEX = 0;
    static constexpr size_t DOES_NOT_CAST_SHADOW_INDEX = 3;

    constexpr bool CastsShadow() const {
        return parameters[DOES_NOT_CAST_SHADOW_INDEX] != 0.0f;
    }
    constexpr void SetCastsShadow(bool casts_shadows) {
        parameters[DOES_NOT_CAST_SHADOW_INDEX] = casts_shadows ? 0.0f : 1.0f;
    }
};
}  // namespace verna

#endif