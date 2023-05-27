#ifndef VERNA_CAMERA_HPP
#define VERNA_CAMERA_HPP

#include <viverna/maths/Mat4f.hpp>
#include <viverna/maths/MathUtils.hpp>
#include <viverna/maths/Quaternion.hpp>

namespace verna {
struct Camera {
    Vec3f position;
    Quaternion rotation;
    float fovy;
    float aspect_ratio;
    float near_plane;
    float far_plane;

    constexpr Camera() :
        fovy(maths::Radians(45.0f)),
        aspect_ratio(16.0f / 9.0f),
        near_plane(0.15),
        far_plane(1000.0f) {}
    constexpr Camera(float fovy_,
                     float aspect_ratio_,
                     float near_plane_,
                     float far_plane_) :
        fovy(fovy_),
        aspect_ratio(aspect_ratio_),
        near_plane(near_plane_),
        far_plane(far_plane_) {}

    constexpr Vec3f Forward() const {
        return rotation.Rotate(Vec3f::UnitZ()).Normalized();
    }

    constexpr Mat4f GetViewMatrix() const {
        Vec3f forward = Forward();
        return Mat4f::LookAt(position, position + forward, Vec3f::UnitY());
    }
    Mat4f GetProjectionMatrix() const;
    Vec3f ToWorldCoords(unsigned screen_x,
                        unsigned screen_y,
                        float camera_dist) const;
    constexpr void ResetTransform() {
        rotation = Quaternion();
        position = Vec3f();
    }
    void SetAspectRatioFromWindowSize();
};
}  // namespace verna

#endif