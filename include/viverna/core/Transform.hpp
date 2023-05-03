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

    constexpr Vec3f Forward() const {
        return rotation.Rotate(Vec3f::UnitZ()).Normalized();
    }
    constexpr Vec3f Right() const {
        return Vec3f::UnitY().Cross(Forward()).Normalized();
    }
    constexpr Vec3f Up() const { return Forward().Cross(Right()).Normalized(); }

    constexpr Mat4f GetMatrix() const {
        Mat4f t_r = rotation.AsMatrix();
        t_r[12] = position.x;
        t_r[13] = position.y;
        t_r[14] = position.z;

        Mat4f s;
        s[0] = scale.x;
        s[5] = scale.y;
        s[10] = scale.z;
        s[15] = 1.0f;

        return t_r * s;
    }

    void LookAt(const Vec3f& pos, const Vec3f& up);

    constexpr Vec3f Apply(const Vec3f& v) const {
        return position + rotation.Rotate(scale.ComponentProduct(v));
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