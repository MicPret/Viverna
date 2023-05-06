#ifndef VERNA_GPU_FRAME_DATA_HPP
#define VERNA_GPU_FRAME_DATA_HPP

#include "CameraData.hpp"
#include "DirectionLightData.hpp"

#include <array>
#include <cstdint>

namespace verna::gpu {
struct FrameData {
    CameraData camera_data;
    DirectionLightData direction_light;

    static constexpr uint32_t BLOCK_BINDING = 0;
    static constexpr const char BLOCK_NAME[] = "FrameData";
};
}  // namespace verna::gpu

#endif