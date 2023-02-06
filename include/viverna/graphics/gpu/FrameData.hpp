#ifndef VIVERNA_GPU_FRAME_DATA_HPP
#define VIVERNA_GPU_FRAME_DATA_HPP

#include "CameraData.hpp"
// TODO #include <string_view>

namespace verna::gpu {
struct FrameData {
    CameraData camera_data;
    static constexpr uint32_t BLOCK_BINDING = 0;
    static constexpr const char BLOCK_NAME[] = "FrameData";
};
}  // namespace verna::gpu

#endif