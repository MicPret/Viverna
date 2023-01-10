#ifndef VERNA_TRANSFORM_HPP
#define VERNA_TRANSFORM_HPP

#include <viverna/maths/Mat4f.hpp>
#include <viverna/maths/Quaternion.hpp>
#include <viverna/maths/Vec3f.hpp>

namespace verna {
struct Transform {
    Vec3f position;
    Quaternion rotation;
    Vec3f scale;

    constexpr Transform() : scale(1.0f, 1.0f, 1.0f) {}

    Vec3f Forward() const;
    Vec3f Right() const;
    Vec3f Up() const;

    constexpr Mat4f GetMatrix() const {
        Mat4f pos_scale;

        pos_scale[0] = scale.x;
        pos_scale[5] = scale.y;
        pos_scale[10] = scale.z;
        pos_scale[12] = position.x;
        pos_scale[13] = position.y;
        pos_scale[14] = position.z;
        pos_scale[15] = 1.0f;

        Mat4f rot;
        const Quaternion& r = rotation;
        rot[0] = 1.0f - 2.0f * (r.y * r.y + r.z * r.z);
        rot[1] = 2.0f * (r.x * r.y + r.w * r.z);
        rot[2] = 2.0f * (r.x * r.z - r.w * r.y);
        rot[4] = 2.0f * (r.x * r.y - r.w * r.z);
        rot[5] = 1.0f - 2.0f * (r.x * r.x + r.z * r.z);
        rot[6] = 2.0f * (r.y * r.z + r.w * r.x);
        rot[8] = 2.0f * (r.x * r.z + r.w * r.y);
        rot[9] = 2.0f * (r.y * r.z - r.w * r.x);
        rot[10] = 1.0f - 2.0f * (r.x * r.x + r.y * r.y);
        rot[15] = 1.0f;

        return pos_scale * rot;  // TODO optimize!!!
    }
};
}  // namespace verna

#endif