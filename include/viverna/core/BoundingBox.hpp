#ifndef VERNA_BOUNDING_BOX_HPP
#define VERNA_BOUNDING_BOX_HPP

#include <viverna/core/Debug.hpp>
#include <viverna/maths/Vec3f.hpp>

#include <array>

namespace verna {

struct Mesh;
struct Transform;
class BoundingSphere;

/**
 * @brief AABB in world coordinates
 *
 */
class BoundingBox {
   public:
    constexpr BoundingBox() : size(Vec3f(1.0f)) {}
    constexpr BoundingBox(const Vec3f& min_pos, const Vec3f& size_) :
        position(min_pos), size(size_) {
        Fix();
    }
    constexpr BoundingBox(const Vec3f& center,
                          float half_width,
                          float half_height,
                          float half_depth) :
        size(2.0f * Vec3f(half_width, half_height, half_depth)) {
        SetCenter(center);
        Fix();
    }
    constexpr Vec3f Center() const { return MinPosition() + HalfSize(); }
    constexpr const Vec3f& MinPosition() const { return position; }
    constexpr Vec3f& MinPosition() { return position; }
    constexpr Vec3f MaxPosition() const { return MinPosition() + Size(); }
    constexpr const Vec3f& Size() const { return size; }
    constexpr Vec3f HalfSize() const { return 0.5f * Size(); }
    constexpr float Width() const { return size.x; }
    constexpr float Height() const { return size.y; }
    constexpr float Depth() const { return size.z; }
    constexpr bool Contains(const Vec3f& point) const {
        auto min = MinPosition();
        auto max = MaxPosition();
        return (point.x >= min.x) && (point.y >= min.y) && (point.z >= min.z)
               && (point.x <= max.x) && (point.y <= max.y)
               && (point.z <= max.z);
    }
    constexpr bool IsCompletelyInside(const BoundingBox& outer) {
        std::array vertices = Vertices();
        for (const Vec3f& v : vertices)
            if (!outer.Contains(v))
                return false;
        return true;
    }
    constexpr void ScaleFromMinPosition(const Vec3f& scale) {
        size = Vec3f(scale.x * size.x, scale.y * size.y, scale.z * size.z);
    }
    constexpr void ScaleFromMinPosition(float scalar) {
        ScaleFromMinPosition(Vec3f(scalar));
    }
    constexpr void ScaleFromCenter(const Vec3f& scale) {
        Vec3f center = Center();
        ScaleFromMinPosition(scale);
        SetCenter(center);
    }
    constexpr void ScaleFromCenter(float scalar) {
        ScaleFromCenter(Vec3f(scalar));
    }
    constexpr void SetCenter(const Vec3f& center) {
        position = center - 0.5f * Size();
    }
    constexpr bool Collides(const BoundingBox& other) const {
        Vec3f b_a = other.MaxPosition() - MinPosition();
        Vec3f a_b = MaxPosition() - other.MinPosition();
        return (b_a.x < 0.0f || b_a.y < 0.0f || b_a.z < 0.0f)
               || (a_b.x >= 0.0 && a_b.y >= 0.0f && a_b.z >= 0.0f);
    }
    bool Collides(const BoundingSphere& sphere) const;
    void Recalculate(const Mesh& mesh, const Transform& transform);
    void Recalculate(const Mesh& mesh);
    /**
     * @brief Computes bounds based on world positions that must fit inside
     *
     * @tparam Vec3fCollection A collection of Vec3f (e.g. std::array,
     * std::vector)
     * @param positions Points in space to fit inside the AABB, in world
     * coordinates
     */
    template <typename Vec3fCollection>
    void Recalculate(Vec3fCollection&& positions) {
        if (positions.begin() == positions.end()) {
            VERNA_LOGW(
                "Called BoundingBox::Recalculate on empty collection of "
                "Vec3f!");
            position = Vec3f();
            size = Vec3f();
            return;
        }
        Vec3f min = *(positions.begin());
        Vec3f max = min;
        for (const Vec3f& pos : positions) {
            min = Vec3f::Min(min, pos);
            max = Vec3f::Max(max, pos);
        }
        position = min;
        size = max - min;
    }

   private:
    Vec3f position;
    Vec3f size;
    constexpr void Fix() {
        if (size.x < 0) {
            position.x -= size.x;
            size.x = -size.x;
        }
        if (size.y < 0) {
            position.y -= size.y;
            size.y = -size.y;
        }
        if (size.z < 0) {
            position.z -= size.z;
            size.z = -size.z;
        }
    }
    // Unordered
    constexpr std::array<Vec3f, 8> Vertices() const {
        const Vec3f& min = position;
        Vec3f max = MaxPosition();
        return {min,
                {min.x, min.y, max.z},
                {min.x, max.y, min.z},
                {min.x, max.y, max.z},
                {max.x, min.y, min.z},
                {max.x, min.y, max.z},
                {max.x, max.y, min.z},
                max};
    }
};
}  // namespace verna

#endif