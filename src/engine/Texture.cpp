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
#include <utility>
#include <vector>

namespace verna {
#ifndef NDEBUG
static ResourceTracker<TextureId::id_type> texture_tracker("Texture");
#endif
static SparseSet<TextureId::id_type> textures_mapper;
static std::vector<std::string> texture_names;
static std::vector<Image> images;

static GLuint GenTexture(const void* pixels, int width, int height);
static TextureId LoadTextureFromBuffer(const void* buffer,
                                       int width,
                                       int height);

TextureId LoadTexture(const std::filesystem::path& texture_path,
                      TextureLoadConfig config) {
    std::string name = texture_path.string();
    for (size_t i = 0; i < texture_names.size(); i++) {
        if (name == texture_names[i]) {
            const auto& ids = textures_mapper.GetDense();
            return TextureId(ids[i]);
        }
    }

    TextureId result;
    std::filesystem::path fullpath = "textures" / texture_path;
    Image img = Image::Load(fullpath);
    if (!img.IsValid()) {
        VERNA_LOGE("LoadImage failed: " + fullpath.string());
        return result;
    }
    result = LoadTextureFromImage(img);
    if (result.IsValid()) {
        textures_mapper.Add(result.id);
        texture_names.push_back(name);
        if (config.flags & TextureLoadConfig::KeepInCpuMemory)
            images.push_back(std::move(img));
        else
            images.emplace_back();
    } else {
        VERNA_LOGE("LoadTextureFromImage failed: " + fullpath.string());
    }
    return result;
}

TextureId LoadTextureFromColor(Color4f color, TextureLoadConfig config) {
    Color4u8 col_u8;
    col_u8.red = static_cast<uint8_t>(color.red * 255.0f);
    col_u8.green = static_cast<uint8_t>(color.green * 255.0f);
    col_u8.blue = static_cast<uint8_t>(color.blue * 255.0f);
    col_u8.alpha = static_cast<uint8_t>(color.alpha * 255.0f);
    return LoadTextureFromColor(col_u8, config);
}

TextureId LoadTextureFromColor(Color4u8 color, TextureLoadConfig config) {
    TextureId result;
    if (config.flags & TextureLoadConfig::KeepInCpuMemory) {
        Image img = Image::LoadFromColor(color, 1, 1);
        if (!img.IsValid()) {
            VERNA_LOGE("Image::LoadFromColor failed!");
            return result;
        }
        result = LoadTextureFromImage(img);
        textures_mapper.Add(result.id);
        texture_names.emplace_back();
        images.push_back(std::move(img));
    } else {
        result = LoadTextureFromBuffer(color.Data(), 1, 1);
    }
    return result;
}

TextureId LoadTextureFromImage(const Image& img) {
    if (!img.IsValid()) {
        VERNA_LOGE("LoadTextureFromImage received an invalid Image!");
        return TextureId();
    }
    return LoadTextureFromBuffer(img.Pixels(), img.Width(), img.Height());
}

void FreeTexture(TextureId texture) {
    if (!texture.IsValid())
        return;
    SparseSet<TextureId::id_type>::index_t index;
    if (textures_mapper.GetIndex(texture.id, index)) {
        textures_mapper.Remove(texture.id);
        texture_names[index].clear();
        texture_names[index].shrink_to_fit();
        images[index].Clear();
    }
    glDeleteTextures(1, &texture.id);
#ifndef NDEBUG
    texture_tracker.Remove(texture.id);
#endif
}

static TextureId LoadTextureFromBuffer(const void* buffer,
                                       int width,
                                       int height) {
    TextureId result;
    if (width <= 0 || height <= 0 || buffer == nullptr) {
        VERNA_LOGE("LoadTextureFromBuffer failed!");
        return result;
    }
    result.id = GenTexture(buffer, width, height);
#ifndef NDEBUG
    texture_tracker.Push(result.id);
#endif
    return result;
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

Color4u8 GetTextureColor(TextureId texture, int pixel_x, int pixel_y) {
    SparseSet<TextureId::id_type>::index_t index;
    if (textures_mapper.GetIndex(texture.id, index)) {
        const Image& img = images[index];
        if (img.IsValid()) {
            auto pixels = img.Pixels();
            int x = pixel_x % img.Width();
            int y = pixel_y % img.Height();
            auto pixel_coord = y * img.Width() + x;
            return pixels[pixel_coord];
        }
    }

    Color4u8 res;
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture.id, 0);
    glReadPixels(pixel_x, pixel_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, res.Data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    return res;
}
}  // namespace verna
