#ifndef VERNA_TEXTURE_HPP
#define VERNA_TEXTURE_HPP

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

TextureId LoadTextureFromFile(const std::filesystem::path& path);
TextureId LoadTextureFromColor(float red, float green, float blue, float alpha);
TextureId LoadTextureFromColor(uint8_t red,
                               uint8_t green,
                               uint8_t blue,
                               uint8_t alpha);
TextureId LoadTextureFromBuffer(const void* buffer, int width, int height);
void FreeTexture(TextureId texture);

}  // namespace verna

#endif