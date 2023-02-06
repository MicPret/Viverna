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
        Mat4f t = Mat4f::Identity();
        t[12] = position.x;
        t[13] = position.y;
        t[14] = position.z;

        Mat4f s;
        s[0] = scale.x;
        s[5] = scale.y;
        s[10] = scale.z;
        s[15] = 1.0f;

        return t * rotation.AsMatrix() * s;  // TODO optimize!!!
    }

    constexpr Vec3f Apply(const Vec3f& v) const {
        Vec3f output;
        output.x = v.x * scale.x;
        output.y = v.y * scale.y;
        output.z = v.z * scale.z;
        return position + rotation.Rotate(output);
    }

    static constexpr Transform Lerp(const Transform& a,
                                    const Transform& b,
                                    float t) {
        Transform output;
        output.position = Vec3f::Lerp(a.position, b.position, t);
        output.rotation = Quaternion::Lerp(a.rotation, b.rotation, t);
        output.scale = Vec3f::Lerp(a.scale, b.scale, t);
        return output;
    }
};
}  // namespace verna

#endif