#ifndef VIVERNA_GPU_FRAME_DATA_HPP
#define VIVERNA_GPU_FRAME_DATA_HPP

#include "CameraData.hpp"
#include "PointLightData.hpp"

// TODO #include <string_view>
#include <array>

namespace verna::gpu {
struct FrameData {
    static constexpr unsigned MAX_POINT_LIGHTS = 16;

    CameraData camera_data;
    std::array<PointLightData, MAX_POINT_LIGHTS> point_lights;
    int32_t num_point_lights;

    static constexpr uint32_t BLOCK_BINDING = 0;
    static constexpr const char BLOCK_NAME[] = "FrameData";
};
}  // namespace verna::gpu

#endif