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

    constexpr Vec3f Forward() const {
        return rotation.Rotate(Vec3f::UnitZ()).Normalized();
    }

    constexpr Mat4f GetViewMatrix() const {
        Vec3f forward = Forward();
        Vec3f up = forward.Cross(Vec3f::UnitX());
        return Mat4f::LookAt(position, position + forward, up);
    }
    Mat4f GetProjectionMatrix() const;
    Vec3f ToWorldCoords(unsigned screen_x,
                        unsigned screen_y,
                        float camera_dist) const;
    constexpr void Reset() {
        rotation = Quaternion();
        position = Vec3f();
    }

    static Camera& GetActive();
    static void SetActive(Camera& camera);
};
}  // namespace verna

#endif