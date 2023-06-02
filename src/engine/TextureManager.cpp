#include <viverna/graphics/TextureManager.hpp>
#include <viverna/core/Debug.hpp>

#if defined(VERNA_DESKTOP)
#include <glad/gl.h>
#elif defined(VERNA_ANDROID)
#include <GLES3/gl32.h>
#else
#error Platform not supported!
#endif

#include <set>
#include <utility>

namespace verna {
static TextureId GenTextureFromBuffer(const void* buffer,
                                      int width,
                                      int height);

TextureManager::~TextureManager() {
    const auto& textures = mapper.GetDense();
    for (size_t i = 0; i < textures.size(); i++) {
        TextureId tex(textures[i]);
        if (mapper.Contains(tex.id)) {
            std::string name = names[i];
            if (name.empty()) {
                auto col = GetTextureColor(tex, 0, 0);
                name = '[' + std::to_string(static_cast<unsigned>(col.red))
                       + ", " + std::to_string(static_cast<unsigned>(col.green))
                       + ", " + std::to_string(static_cast<unsigned>(col.blue))
                       + ", " + std::to_string(static_cast<unsigned>(col.alpha))
                       + ']';
            }
            VERNA_LOGE("Texture not freed: " + name + " ("
                       + std::to_string(tex.id) + ')');
        }
    }
}

TextureId TextureManager::LoadTexture(const std::filesystem::path& texture_path,
                                      TextureLoadConfig config) {
    std::string name = texture_path.string();
    VERNA_LOGI("Loading texture " + name + "...");
    const auto& ids = mapper.GetDense();
    for (size_t i = 0; i < ids.size(); i++)
        if (mapper.Contains(ids[i]) && name == names[i])
            return TextureId(ids[i]);

    TextureId result;
    std::filesystem::path fullpath = "textures" / texture_path;
    Image img = Image::Load(fullpath);
    if (!img.IsValid()) {
        VERNA_LOGE("Image::Load failed: " + fullpath.string());
        return result;
    }
    result = LoadTextureFromImage(img);
    if (!result.IsValid()) {
        VERNA_LOGE("LoadTextureFromImage failed: " + fullpath.string());
        return result;
    }
    VERNA_LOGI(name + " successfully loaded!");
    if (config.flags & TextureLoadConfig::KeepInCpuMemory)
        images.back() = std::move(img);
    names.back() = std::move(name);
    return result;
}

TextureId TextureManager::LoadTextureFromColor(const Color4f& color,
                                               TextureLoadConfig config) {
    Color4u8 col_u8(static_cast<uint8_t>(color.red * 255.0f),
                    static_cast<uint8_t>(color.green * 255.0f),
                    static_cast<uint8_t>(color.blue * 255.0f),
                    static_cast<uint8_t>(color.alpha * 255.0f));
    return LoadTextureFromColor(col_u8, config);
}

TextureId TextureManager::LoadTextureFromColor(Color4u8 color,
                                               TextureLoadConfig config) {
    const auto& dense = mapper.GetDense();
    for (size_t i = 0; i < dense.size(); i++) {
        TextureId t(dense[i]);
        if (IsColorTexture(t) && GetTextureColor(t, 0, 0) == color)
            return t;
    }
    TextureId result;
    auto keep_in_ram = config.flags & TextureLoadConfig::KeepInCpuMemory;
    if (keep_in_ram == 0) {
        result = GenTextureFromBuffer(color.Data(), 1, 1);
        if (result.IsValid())
            AddElement(result.id);
        return result;
    }
    Image img = Image::LoadFromColor(color, 1, 1);
    if (!img.IsValid()) {
        VERNA_LOGE("Image::LoadFromColor failed!");
        return result;
    }
    result = LoadTextureFromImage(img);
    if (result.IsValid())
        images.back() = std::move(img);

    return result;
}

TextureId TextureManager::LoadTextureFromImage(const Image& img) {
    TextureId result;
    if (!img.IsValid()) {
        VERNA_LOGE("LoadTextureFromImage received an invalid Image!");
        return result;
    }
    result = GenTextureFromBuffer(img.Pixels(), img.Width(), img.Height());
    if (!result.IsValid())
        VERNA_LOGE("GenTextureFromBuffer failed inside LoadTextureFromImage!");
    else
        AddElement(result.id);
    return result;
}

void TextureManager::FreeTexture(TextureId texture) {
    if (!texture.IsValid())
        return;
    SparseSet<TextureId::id_type>::index_t index;
    if (mapper.GetIndex(texture.id, index)) {
        auto id_string = std::to_string(texture.id);
        RemoveElement(texture.id);
        glDeleteTextures(1, &texture.id);
    } else {
        VERNA_LOGI("Called FreeTexture on missing texture: "
                   + std::to_string(texture.id));
    }
}

void TextureManager::FreeLoadedTextures() {
    auto dense = mapper.GetDense();
    std::set<TextureId::id_type> to_free_set;
    SparseSet<TextureId::id_type>::index_t index;
    for (TextureId::id_type t : dense) {
        if (mapper.GetIndex(t, index)) {
            to_free_set.insert(t);
        }
    }
    std::vector to_free(to_free_set.begin(), to_free_set.end());
    glDeleteTextures(to_free.size(), to_free.data());
    mapper.Clear();
    names.clear();
    images.clear();
}

std::filesystem::path TextureManager::GetTexturePath(TextureId texture) const {
    SparseSet<TextureId::id_type>::index_t index;
    using path_t = std::filesystem::path;
    return mapper.GetIndex(texture.id, index) ? path_t(names[index]) : path_t();
}

Color4u8 TextureManager::GetTextureColor(TextureId texture,
                                         int pixel_x,
                                         int pixel_y) const {
    SparseSet<TextureId::id_type>::index_t index;
    if (mapper.GetIndex(texture.id, index)) {
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

bool TextureManager::IsColorTexture(TextureId texture) const {
    SparseSet<TextureId::id_type>::index_t index;
    if (!mapper.GetIndex(texture.id, index))
        return false;
    return names[index].empty();
}

void TextureManager::AddElement(TextureId::id_type id) {
    mapper.Add(id);
    names.emplace_back();
    images.emplace_back();
}

void TextureManager::RemoveElement(TextureId::id_type id) {
    SparseSet<TextureId::id_type>::index_t index;
    if (mapper.GetIndex(id, index)) {
        mapper.Remove(id);
        names[index].clear();
        names[index].shrink_to_fit();
        images[index].Clear();
    }
}

// static functions

TextureId GenTextureFromBuffer(const void* buffer, int width, int height) {
    TextureId result;
    if (width <= 0 || height <= 0 || buffer == nullptr) {
        VERNA_LOGE("GenTextureFromBuffer failed!");
        return result;
    }

    glGenTextures(1, &result.id);
    glBindTexture(GL_TEXTURE_2D, result.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // TODO GL_NEAREST or GL_LINEAR? Should specify in function declaration?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA,
                    GL_UNSIGNED_BYTE, buffer);

    return result;
}
}  // namespace verna
