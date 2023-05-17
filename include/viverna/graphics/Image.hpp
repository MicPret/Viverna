#ifndef VERNA_IMAGE_HPP
#define VERNA_IMAGE_HPP

#include "Color4.hpp"
#include <viverna/maths/Vec2i.hpp>

#include <filesystem>

namespace verna {

/**
 * @brief Image info
 *
 */
class Image {
   public:
    using color_t = Color4u8;
    Image();
    Image(const Image& other);
    Image(Image&& other);
    Image& operator=(const Image& other);
    Image& operator=(Image&& other);
    ~Image();
    bool IsValid() const;
    int Width() const;
    int Height() const;
    int Area() const;
    Vec2i Size() const;
    const color_t* Pixels() const;
    void Clear();
    static Image Load(const std::filesystem::path& image_path);
    static Image LoadFromColor(color_t color, int width, int height);

   private:
    int width;
    int height;
    color_t* pixels;
    void ClearPixels();
    static Image LoadFromBuffer(const uint8_t* buffer, size_t size);
};

}  // namespace verna

#endif