#ifndef VERNA_TEXTURE_HPP
#define VERNA_TEXTURE_HPP

#include "Image.hpp"

#include <cstdint>
#include <filesystem>

namespace verna {

struct TextureId {
    using id_type = uint32_t;
    id_type id;
    constexpr TextureId() : id(0u) {}
    constexpr TextureId(id_type id_) : id(id_) {}
    constexpr bool IsValid() const { return id != 0u; }
};
constexpr bool operator==(TextureId a, TextureId b) {
    return a.id == b.id;
}
constexpr bool operator!=(TextureId a, TextureId b) {
    return !(a == b);
}

/**
 * @brief Loads a texture from its asset file
 *
 * @param texture_path Filepath of a texture asset, excluding "assets/textures"
 * @return Texture identifier, must be freed with FreeTexture
 */
TextureId LoadTexture(const std::filesystem::path& texture_path);
TextureId LoadTextureFromColor(float red, float green, float blue, float alpha);
TextureId LoadTextureFromColor(uint8_t red,
                               uint8_t green,
                               uint8_t blue,
                               uint8_t alpha);
TextureId LoadTextureFromImage(ImageId img);
TextureId LoadTextureFromImageInfo(const Image& img_info);
void FreeTexture(TextureId texture);

}  // namespace verna

#endif