#ifndef VERNA_BOUNDING_BOX_HPP
#define VERNA_BOUNDING_BOX_HPP

#include <viverna/maths/Vec3f.hpp>

#include <vector>

namespace verna {

struct Mesh;
struct Transform;

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
    constexpr void SetCenter(const Vec3f& center) {
        position = center - 0.5f * Size();
    }
    void Recalculate(const Mesh& mesh, const Transform& transform);
    void Recalculate(const std::vector<Vec3f>& world_coords);

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
};
}  // namespace verna

#endif