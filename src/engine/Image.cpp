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

#include <vector>

namespace verna {
static std::vector<Image> loaded_images;
static std::vector<ImageId> loaded_ids;
static ImageId::id_type next_id = 1;

static Image LoadImageFromBuffer(const std::vector<char>& buffer);
static void FreeImageImpl(Image& img);
static size_t GetImageIndex(ImageId img_id);

ImageId LoadImage(const std::filesystem::path& image_path) {
    auto raw = LoadRawAsset(image_path);
    auto img = LoadImageFromBuffer(raw);
    if (img.pixels == nullptr) {
        VERNA_LOGE("LoadImageFromBuffer failed: " + image_path.string());
        return ImageId();
    }
    ImageId output(next_id++);
    loaded_images.push_back(img);
    loaded_ids.push_back(output);
    return output;
}

void FreeImage(ImageId image_id) {
    size_t index = GetImageIndex(image_id);
    if (index >= loaded_ids.size()) {
        VERNA_LOGW("FreeImage failed: no image found with ID "
                   + std::to_string(image_id.id));
        return;
    }
    FreeImageImpl(loaded_images[index]);
    loaded_ids.erase(loaded_ids.begin() + index);
    loaded_images.erase(loaded_images.begin() + index);
}

Image GetImageInfo(ImageId image_id) {
    size_t index = GetImageIndex(image_id);
    if (index >= loaded_ids.size()) {
        VERNA_LOGE("GetImageInfo failed: no image found with ID "
                   + std::to_string(image_id.id));
        return Image();
    }
    return loaded_images[index];
}

static size_t GetImageIndex(ImageId img_id) {
    size_t i;
    for (i = 0; i < loaded_ids.size(); i++)
        if (loaded_ids[i] == img_id)
            break;
    return i;
}

#if defined(VERNA_ANDROID)
Image LoadImageFromBuffer(const std::vector<char>& buffer) {
    AImageDecoder* decoder;
    int result =
        AImageDecoder_createFromBuffer(buffer.data(), buffer.size(), &decoder);
    if (result != ANDROID_IMAGE_DECODER_SUCCESS) {
        VERNA_LOGE("AImageDecoder_createFromBuffer failed!");
        AImageDecoder_delete(decoder);
        return Image();
    }
    result = AImageDecoder_setAndroidBitmapFormat(
        decoder, ANDROID_BITMAP_FORMAT_RGBA_8888);
    if (result != ANDROID_IMAGE_DECODER_SUCCESS) {
        VERNA_LOGE("AImageDecoder_setAndroidBitmapFormat failed: "
                   + std::to_string(result));
        AImageDecoder_delete(decoder);
        return Image();
    }
    auto header_info = AImageDecoder_getHeaderInfo(decoder);
    Image output;
    output.height = AImageDecoderHeaderInfo_getHeight(header_info);
    output.width = AImageDecoderHeaderInfo_getWidth(header_info);
    constexpr size_t bytes_per_pixel = 4;
    size_t stride = std::max(bytes_per_pixel * output.width,
                             AImageDecoder_getMinimumStride(decoder));
    size_t buffer_size = stride * output.height;
    void* pixel_buffer = std::malloc(buffer_size);
    output.pixels = static_cast<uint8_t*>(pixel_buffer);
    result =
        AImageDecoder_decodeImage(decoder, output.pixels, stride, buffer_size);
    AImageDecoder_delete(decoder);
    if (result != ANDROID_IMAGE_DECODER_SUCCESS) {
        VERNA_LOGE("AImageDecoder_setAndroidBitmapFormat failed: "
                   + std::to_string(result));
        std::free(pixel_buffer);
        return Image();
    }
    return output;
}
void FreeImageImpl(Image& img) {
    std::free(img.pixels);
    img = Image();
}
#elif defined(VERNA_DESKTOP)
static bool StbiSetFlip();
Image LoadImageFromBuffer(const std::vector<char>& buffer) {
    [[maybe_unused]] static bool init = StbiSetFlip();
    auto b = reinterpret_cast<const stbi_uc*>(buffer.data());
    Image output;
    int comp;
    output.pixels = stbi_load_from_memory(b, buffer.size(), &output.width,
                                          &output.height, &comp, 4);
    VERNA_LOGE_IF(output.pixels == nullptr, "stbi_load_from_memory failed!");
    return output;
}
void FreeImageImpl(Image& img) {
    stbi_image_free(img.pixels);
    img = Image();
}
bool StbiSetFlip() {
    stbi_set_flip_vertically_on_load(true);
    return true;
}
#endif

}  // namespace verna