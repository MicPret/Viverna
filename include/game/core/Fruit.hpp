#ifndef VERNA_GAME_FRUIT_HPP
#define VERNA_GAME_FRUIT_HPP

#include <viverna/core/BoundingBox.hpp>
#include <viverna/core/BoundingSphere.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Shader.hpp>

#include <game/data/BufferView.hpp>

namespace snake {
class Fruit {
   public:
    Fruit(float distance);
    void Setup();
    void Dispose();
    void Render(verna::ShaderId shader);
    void Recalculate(BufferView<verna::BoundingSphere> sphere_cols,
                     BufferView<verna::BoundingBox> box_cols,
                     verna::Vec2f bounds_min,
                     verna::Vec2f bounds_max);
    const auto& Collider() const { return collider; }

   private:
    verna::Transform transform;
    verna::Mesh mesh;
    verna::Material material;
    verna::BoundingSphere collider;
};
}  // namespace snake

#endif