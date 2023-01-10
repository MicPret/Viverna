#ifndef VERNA_GPU_CAMERA_DATA_HPP
#define VERNA_GPU_CAMERA_DATA_HPP

#include <cmath>
#include <string_view>
#include <viverna/maths/Mat4f.hpp>

namespace verna::gpu {
struct CameraData {
    Mat4f projection_matrix;
    Mat4f view_matrix;
    static constexpr uint32_t BLOCK_BINDING = 0;
    static constexpr const char BLOCK_NAME[] = "CameraData";
};
}  // namespace verna::gpu

#endif