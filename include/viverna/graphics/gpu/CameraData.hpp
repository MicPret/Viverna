#ifndef VERNA_GPU_CAMERA_DATA_HPP
#define VERNA_GPU_CAMERA_DATA_HPP

#include <viverna/graphics/Camera.hpp>
#include <viverna/maths/Mat4f.hpp>

namespace verna::gpu {
struct CameraData {
    Mat4f projection_matrix;
    Mat4f view_matrix;
    Mat4f pv_matrix;
    float near;
    float far;
    float aspect_ratio;
    float fovy;
    CameraData() = default;
    inline CameraData(const Camera& cam) :
        projection_matrix(cam.GetProjectionMatrix()),
        view_matrix(cam.GetViewMatrix()),
        near(cam.near_plane),
        far(cam.far_plane),
        aspect_ratio(cam.aspect_ratio),
        fovy(cam.fovy) {
        pv_matrix = projection_matrix * view_matrix;
    }
};
}  // namespace verna::gpu

#endif