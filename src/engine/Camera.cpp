#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/Window.hpp>
#include <viverna/maths/Vec4f.hpp>

namespace verna {

Mat4f Camera::GetProjectionMatrix() const {
    return Mat4f::Perspective(fovy, aspect_ratio, near_plane, far_plane);
}

Vec3f Camera::ToWorldCoords(unsigned screen_x,
                            unsigned screen_y,
                            float camera_dist) const {
    unsigned reverse_y = WindowHeight() - screen_y;
    unsigned center_x = WindowWidth() / 2;
    unsigned center_y = WindowHeight() / 2;
    float ndc_x =
        static_cast<float>(screen_x) / static_cast<float>(center_x) - 1.0f;
    float ndc_y =
        static_cast<float>(reverse_y) / static_cast<float>(center_y) - 1.0f;
    Vec4f ndc_origin = Vec4f(ndc_x, ndc_y, 1.0f, 1.0f);
    Mat4f proj = GetProjectionMatrix();
    Mat4f view = GetViewMatrix();
    Mat4f proj_view = proj * view;
    Mat4f i_pv = proj_view.Inverted();
    Vec4f result = i_pv * ndc_origin;
    float inverse_w = 1.0f / result.w;
    Vec3f origin = inverse_w * result.Xyz();
    Vec3f ray_dir = (origin - position).Normalized();
    Vec3f output = origin + (camera_dist * ray_dir);
    return output;
}

void Camera::SetAspectRatioFromWindowSize() {
    auto width = static_cast<float>(WindowWidth());
    auto height = static_cast<float>(WindowHeight());
    aspect_ratio = width / height;
}

}  // namespace verna