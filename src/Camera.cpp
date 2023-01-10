#include <viverna/graphics/Camera.hpp>

namespace verna {

static Camera* active_camera;

Camera& Camera::GetActive() {
    // TODO better default cam
    static Camera c(0.785398f, 16.0f / 10.0f, 0.15f, 1000.0f);
    if (active_camera == nullptr)  // only first call
        active_camera = &c;
    return *active_camera;
}

void Camera::SetActive(Camera& camera) {
    active_camera = &camera;
}

Mat4f Camera::GetProjectionMatrix() const {
    return Mat4f::Perspective(fovy, aspect_ratio, near_plane, far_plane);
}
}  // namespace verna