#include <viverna/graphics/Image.hpp>
#include <viverna/core/Assets.hpp>
#include <viverna/core/Debug.hpp>

#if defined(VERNA_ANDROID)
#include <android/imagedecoder.h>
#include <cstdlib>
#elif defined(VERNA_DESKTOP)
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#else
#error Platform not supported!
#endif

#include <algorithm>
#include <cstdlib>
#include <utility>

namespace verna {

Image::Image() : width(0), height(0), pixels(nullptr) {}

Image::Image(const Image& other) :
    width(other.width), height(other.height), pixels(nullptr) {
    if (!other.IsValid())
        return;
    size_t area = static_cast<size_t>(other.Area());
    void* data = std::malloc(sizeof(color_t) * area);
    pixels = static_cast<color_t*>(data);
    std::copy_n(other.pixels, area, pixels);
}

Image::Image(Image&& other) :
    width(other.width), height(other.height), pixels(other.pixels) {
    other.pixels = nullptr;
    other.width = 0;
    other.height = 0;
}

Image& Image::operator=(const Image& other) {
    size_t new_size = static_cast<size_t>(other.Area());
    void* p = std::realloc(pixels, sizeof(color_t) * new_size);
    pixels = static_cast<color_t*>(p);
    std::copy_n(other.pixels, new_size, pixels);
    width = other.width;
    height = other.height;
    return *this;
}

Image& Image::operator=(Image&& other) {
    ClearPixels();
    width = other.width;
    height = other.height;
    pixels = other.pixels;
    other.pixels = nullptr;
    return *this;
}

Image::~Image() {
    Clear();
}

int Image::Width() const {
    return width;
}

int Image::Height() const {
    return height;
}

int Image::Area() const {
    return width * height;
}

Vec2i Image::Size() const {
    return Vec2i(width, height);
}

const Image::color_t* Image::Pixels() const {
    return pixels;
}

bool Image::IsValid() const {
    return (pixels != nullptr) && (width > 0) && (height > 0);
}

void Image::Clear() {
    ClearPixels();
    width = 0;
    height = 0;
}

void Image::ClearPixels() {
    std::free(pixels);
    pixels = nullptr;
}

Image Image::Load(const std::filesystem::path& image_path) {
    auto raw = LoadRawAsset(image_path);
    auto ptr = reinterpret_cast<const uint8_t*>(raw.data());
    auto result = LoadFromBuffer(ptr, raw.size());
    VERNA_LOGE_IF(!result.IsValid(),
                  "LoadImageFromBuffer failed: " + image_path.string());
    return result;
}

Image Image::LoadFromColor(color_t color, int width, int height) {
    Image result;
    if (width <= 0 || height <= 0)
        return result;
    int size = width * height;
    void* p = std::malloc(sizeof(color_t) * size);
    if (p != nullptr) {
        result.width = width;
        result.height = height;
        result.pixels = static_cast<color_t*>(p);
        for (int i = 0; i < size; i++)
            result.pixels[i] = color;
    }
    return result;
}

#if defined(VERNA_ANDROID)
Image Image::LoadFromBuffer(const uint8_t* buffer, size_t size) {
    Image result;
    AImageDecoder* decoder;
    int err_code = AImageDecoder_createFromBuffer(buffer, size, &decoder);
    if (err_code != ANDROID_IMAGE_DECODER_SUCCESS) {
        VERNA_LOGE("AImageDecoder_createFromBuffer failed!");
        AImageDecoder_delete(decoder);
        return result;
    }
    err_code = AImageDecoder_setAndroidBitmapFormat(
        decoder, ANDROID_BITMAP_FORMAT_RGBA_8888);
    if (err_code != ANDROID_IMAGE_DECODER_SUCCESS) {
        VERNA_LOGE("AImageDecoder_setAndroidBitmapFormat failed: "
                   + std::to_string(err_code));
        AImageDecoder_delete(decoder);
        return result;
    }
    auto header_info = AImageDecoder_getHeaderInfo(decoder);
    result.height = AImageDecoderHeaderInfo_getHeight(header_info);
    result.width = AImageDecoderHeaderInfo_getWidth(header_info);
    constexpr size_t bytes_per_pixel = 4;
    size_t stride = std::max(bytes_per_pixel * result.width,
                             AImageDecoder_getMinimumStride(decoder));
    size_t buffer_size = stride * result.height;
    void* pixel_buffer = std::malloc(buffer_size);
    err_code =
        AImageDecoder_decodeImage(decoder, pixel_buffer, stride, buffer_size);
    AImageDecoder_delete(decoder);
    if (err_code != ANDROID_IMAGE_DECODER_SUCCESS) {
        VERNA_LOGE("AImageDecoder_setAndroidBitmapFormat failed: "
                   + std::to_string(err_code));
        std::free(pixel_buffer);
        result.width = 0;
        result.height = 0;
    } else {
        result.pixels = static_cast<Image::color_t*>(pixel_buffer);
    }
    return result;
}
#elif defined(VERNA_DESKTOP)
Image Image::LoadFromBuffer(const uint8_t* buffer, size_t size) {
    stbi_set_flip_vertically_on_load(true);
    auto buf = reinterpret_cast<const stbi_uc*>(buffer);
    Image result;
    int comp;
    auto loaded = stbi_load_from_memory(buf, size, &result.width,
                                        &result.height, &comp, 4);
    if (loaded == nullptr) {
        VERNA_LOGE("stbi_load_from_memory failed!");
        result.width = 0;
        result.height = 0;
    } else {
        result.pixels = reinterpret_cast<Image::color_t*>(loaded);
    }
    return result;
}
#endif

}  // namespace verna