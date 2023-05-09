#include <viverna/graphics/Texture.hpp>
#include <viverna/core/Assets.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/data/SparseSet.hpp>
#include <viverna/graphics/Image.hpp>
#include "ResourceTracker.hpp"

#if defined(VERNA_DESKTOP)
#include <glad/gl.h>
#elif defined(VERNA_ANDROID)
#include <GLES3/gl32.h>
#else
#error Platform not supported!
#endif

#include <array>
#include <cstdint>
#include <vector>

namespace verna {
#ifndef NDEBUG
static ResourceTracker<TextureId::id_type> texture_tracker("Texture");
#endif
static SparseSet<TextureId::id_type> textures_mapper;
static std::vector<std::string> texture_names;

static GLuint GenTexture(const void* pixels, int width, int height);
static TextureId LoadTextureFromBuffer(const void* buffer,
                                       int width,
                                       int height);

TextureId LoadTexture(const std::filesystem::path& texture_path) {
    std::string name = texture_path.string();
    for (size_t i = 0; i < texture_names.size(); i++) {
        if (name != texture_names[i])
            continue;
        const auto& ids = textures_mapper.GetDense();
        return TextureId(ids[i]);
    }

    std::filesystem::path fullpath = "textures" / texture_path;
    ImageId img = LoadImage(fullpath);
    if (!img.IsValid()) {
        VERNA_LOGE("LoadImage failed: " + fullpath.string());
        return TextureId();
    }
    auto output = LoadTextureFromImage(img);
    FreeImage(img);
    if (output.IsValid()) {
        textures_mapper.Add(output.id);
        texture_names.push_back(name);
    } else {
        VERNA_LOGE("LoadTextureFromImage failed: " + fullpath.string());
    }
    return output;
}

TextureId LoadTextureFromColor(float red,
                               float green,
                               float blue,
                               float alpha) {
    uint8_t r8 = static_cast<uint8_t>(red * 255.0f);
    uint8_t g8 = static_cast<uint8_t>(green * 255.0f);
    uint8_t b8 = static_cast<uint8_t>(blue * 255.0f);
    uint8_t a8 = static_cast<uint8_t>(alpha * 255.0f);
    return LoadTextureFromColor(r8, g8, b8, a8);
}

TextureId LoadTextureFromColor(uint8_t red,
                               uint8_t green,
                               uint8_t blue,
                               uint8_t alpha) {
    constexpr int width = 2;
    constexpr int height = width;
    constexpr int size = width * height;
    constexpr int num_channels = 4;
    std::array<uint8_t, num_channels * size> data;
    for (int i = 0; i < size; i++) {
        auto index = i * size;
        data[index] = red;
        data[index + 1] = green;
        data[index + 2] = blue;
        data[index + 3] = alpha;
    }
    return LoadTextureFromBuffer(data.data(), width, height);
}

TextureId LoadTextureFromImage(ImageId img) {
    return LoadTextureFromImageInfo(GetImageInfo(img));
}

TextureId LoadTextureFromImageInfo(const Image& img_info) {
    return LoadTextureFromBuffer(img_info.pixels, img_info.width,
                                 img_info.height);
}

void FreeTexture(TextureId texture) {
    if (!texture.IsValid())
        return;
    SparseSet<TextureId::id_type>::index_t index;
    if (textures_mapper.GetIndex(texture.id, index)) {
        textures_mapper.Remove(texture.id);
        texture_names[index].clear();
        texture_names[index].shrink_to_fit();
    }
    glDeleteTextures(1, &texture.id);
#ifndef NDEBUG
    texture_tracker.Remove(texture.id);
#endif
}

static TextureId LoadTextureFromBuffer(const void* buffer,
                                       int width,
                                       int height) {
    if (width <= 0 || height <= 0 || buffer == nullptr) {
        VERNA_LOGE("LoadTextureFromBuffer failed!");
        return TextureId();
    }
    auto texture = GenTexture(buffer, width, height);
#ifndef NDEBUG
    texture_tracker.Push(texture);
#endif
    return TextureId(texture);
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

std::filesystem::path GetTexturePath(TextureId texture) {
    SparseSet<TextureId::id_type>::index_t index;
    return textures_mapper.GetIndex(texture.id, index)
               ? texture_names[index]
               : std::filesystem::path();
}
}  // namespace verna
