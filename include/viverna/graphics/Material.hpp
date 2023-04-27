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
};
}  // namespace verna

#endif