#ifndef VERNA_CAMERA_HPP
#define VERNA_CAMERA_HPP

#include <viverna/maths/Mat4f.hpp>
#include <viverna/maths/Quaternion.hpp>

namespace verna {
struct Camera {
    Vec3f position;
    Quaternion rotation;
    float fovy;
    float aspect_ratio;
    float near_plane;
    float far_plane;

    constexpr Camera(float fovy_,
                     float aspect_ratio_,
                     float near_plane_,
                     float far_plane_) :
        fovy(fovy_),
        aspect_ratio(aspect_ratio_),
        near_plane(near_plane_),
        far_plane(far_plane_) {}

    constexpr Mat4f GetViewMatrix() const {
        Vec3f forward = rotation.Rotate(-Vec3f::UnitZ()).Normalized();
        Vec3f right = forward.Cross(Vec3f::UnitY()).Normalized();
        Vec3f up = right.Cross(forward).Normalized();

        return Mat4f::LookAt(position, right, up, forward);
    }
    Mat4f GetProjectionMatrix() const;

    static Camera& GetActive();
    static void SetActive(Camera& camera);
};
}  // namespace verna

#endif