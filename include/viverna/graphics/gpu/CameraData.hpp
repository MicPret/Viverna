#ifndef VERNA_GPU_CAMERA_DATA_HPP
#define VERNA_GPU_CAMERA_DATA_HPP

#include <viverna/maths/Mat4f.hpp>

namespace verna::gpu {
struct CameraData {
    Mat4f projection_matrix;
    Mat4f view_matrix;
    Mat4f pv_matrix;
};
}  // namespace verna::gpu

#endif