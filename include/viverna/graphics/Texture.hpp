#ifndef VERNA_TEXTURE_HPP
#define VERNA_TEXTURE_HPP

#include "Image.hpp"

#include <array>
#include <cstdint>
#include <filesystem>

namespace verna {

struct TextureId {
    using id_type = uint32_t;
    id_type id;
    constexpr TextureId() : id(0u) {}
    explicit constexpr TextureId(id_type id_) : id(id_) {}
    constexpr bool IsValid() const { return id != 0u; }
};
constexpr bool operator==(TextureId a, TextureId b) {
    return a.id == b.id;
}
constexpr bool operator!=(TextureId a, TextureId b) {
    return !(a == b);
}

struct TextureLoadConfig {
    using flag_t = uint8_t;
    flag_t flags;
    constexpr TextureLoadConfig() : flags(0) {}
    explicit constexpr TextureLoadConfig(flag_t flags_) : flags(flags_) {}
    static constexpr flag_t KeepInCpuMemory = 1;
    // Other stuff like compression format
};

// /**
//  * @brief Loads a texture from its asset file
//  *
//  * @param texture_path Filepath of a texture asset, excluding
//  "assets/textures"
//  * @param config Loading configuration (i.e. keep image data in CPU RAM)
//  * @return Texture identifier, must be freed with FreeTexture
//  */
// TextureId LoadTexture(const std::filesystem::path& texture_path,
//                       TextureLoadConfig config = TextureLoadConfig());

// TextureId LoadTextureFromColor(Color4u8 color,
//                                TextureLoadConfig config =
//                                TextureLoadConfig());

// TextureId LoadTextureFromColor(const Color4f& color,
//                                TextureLoadConfig config =
//                                TextureLoadConfig());

// TextureId LoadTextureFromImage(const Image& img);

// void FreeTexture(TextureId texture);
// /**
//  * @brief Retrieves the filepath of the loaded texture, as passed in
//  * LoadTexture. Won't work if FreeTexture(texture) was called
//  *
//  * @param texture Texture returned by LoadTexture
//  * @return Path of the loaded texture
//  */
// std::filesystem::path GetTexturePath(TextureId texture);

// Color4u8 GetTextureColor(TextureId texture, int pixel_x, int pixel_y);

}  // namespace verna

#endif