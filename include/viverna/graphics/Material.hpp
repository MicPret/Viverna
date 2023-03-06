#ifndef VERNA_MATERIAL_HPP
#define VERNA_MATERIAL_HPP

#include <array>
#include "Texture.hpp"

namespace verna {
struct Material {
    std::array<TextureId, 8> textures = {};
    std::array<float, 4> parameters = {};
};
}  // namespace verna

#endif