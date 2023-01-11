#include <viverna/core/Debug.hpp>
#include <viverna/graphics/Texture.hpp>
#include "GraphicsAPIHelper.hpp"
#include "ResourceTracker.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <array>
#include <cstdint>
#include <vector>

namespace verna {
static ResourceTracker<TextureId::id_type> texture_tracker("Texture");

static GLuint GenTexture(const void* pixels, int width, int height);

TextureId LoadTextureFromFile(const std::filesystem::path& path) {
    int x, y, comp;
    const std::string fullpath = path.string();
    stbi_uc* loaded = stbi_load(fullpath.c_str(), &x, &y, &comp, 4);
    if (loaded == nullptr) {
        VERNA_LOGE("Failed to load texture: " + fullpath);
        return TextureId();  // invalid texture
    }
    GLuint texture = GenTexture(loaded, x, y);
    stbi_image_free(loaded);

    texture_tracker.Push(texture);
    return TextureId(texture);
}

TextureId LoadTextureFromColor(float red,
                               float green,
                               float blue,
                               float alpha) {
    // TODO implement
    uint8_t r8 = static_cast<uint8_t>(red * 255.0f);
    uint8_t g8 = static_cast<uint8_t>(green * 255.0f);
    uint8_t b8 = static_cast<uint8_t>(blue * 255.0f);
    uint8_t a8 = static_cast<uint8_t>(alpha * 255.0f);
    struct col8 {
        uint8_t r, g, b, a;
    };
    col8 pixel{r8, g8, b8, a8};
    std::array<col8, 4> data = {pixel, pixel, pixel, pixel};
    GLuint texture = GenTexture(data.data(), 2, 2);
    texture_tracker.Push(texture);
    return TextureId(texture);
}

void FreeTexture(TextureId texture) {
    glDeleteTextures(1, &texture.id);
    texture_tracker.Remove(texture.id);
}

static GLuint GenTexture(const void* pixels, int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // TODO GL_NEAREST or GL_LINEAR? Should specify in function declaration?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA,
                    GL_UNSIGNED_BYTE, pixels);

    return texture;
}
}  // namespace verna