#ifndef VERNA_IMAGE_HPP
#define VERNA_IMAGE_HPP

#include <cstdint>
#include <filesystem>

namespace verna {

/**
 * @brief Image identifier
 *
 */
struct ImageId {
    using id_type = uint32_t;
    id_type id;
    constexpr ImageId() : id(0u) {}
    constexpr ImageId(id_type id_) : id(id_) {}
    constexpr bool IsValid() const { return id != 0; }
};
constexpr bool operator==(ImageId a, ImageId b) {
    return a.id == b.id;
}
constexpr bool operator!=(ImageId a, ImageId b) {
    return !(a == b);
}

/**
 * @brief Image info
 *
 */
struct Image {
    uint8_t* pixels = nullptr;
    int width = 0;
    int height = 0;
};

/**
 * @brief Loads an Image from the asset folder
 *
 * @param image_path Path of the specified image, excluding "assets/"
 * @return Image identifier, must be freed with FreeImage
 */
ImageId LoadImage(const std::filesystem::path& image_path);
void FreeImage(ImageId image_id);
Image GetImageInfo(ImageId image_id);

}  // namespace verna

#endif