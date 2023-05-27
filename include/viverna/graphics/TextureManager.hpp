#ifndef VERNA_TEXTURE_MANAGER_HPP
#define VERNA_TEXTURE_MANAGER_HPP

#include "Image.hpp"
#include "Texture.hpp"
#include <viverna/data/SparseSet.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace verna {
class TextureManager {
   public:
    TextureManager() = default;
    ~TextureManager();
    TextureManager(const TextureManager&) = delete;
    TextureManager(TextureManager&&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager& operator=(TextureManager&&) = delete;

    TextureId LoadTexture(const std::filesystem::path& texture_path,
                          TextureLoadConfig config);
    TextureId LoadTextureFromColor(Color4u8 color, TextureLoadConfig config);
    TextureId LoadTextureFromColor(const Color4f& color,
                                   TextureLoadConfig config);
    TextureId LoadTextureFromImage(const Image& img);
    void FreeTexture(TextureId texture);
    void FreeLoadedTextures();
    std::filesystem::path GetTexturePath(TextureId texture) const;
    Color4u8 GetTextureColor(TextureId texture, int pixel_x, int pixel_y) const;
    bool IsColorTexture(TextureId texture) const;

   private:
    SparseSet<TextureId::id_type> mapper;
    std::vector<std::string> names;
    std::vector<Image> images;
    void AddElement(TextureId::id_type id);
    void RemoveElement(TextureId::id_type id);
};
}  // namespace verna

#endif
