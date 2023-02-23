#ifndef VERNA_GAME_SNAKE_HPP
#define VERNA_GAME_SNAKE_HPP

#include <viverna/core/BoundingSphere.hpp>
#include <viverna/core/BoundingBox.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Shader.hpp>

#include <game/data/BufferView.hpp>

#include <vector>

namespace snake {
class Snake {
   public:
    Snake(float distance_);
    void Setup();
    void Dispose();
    void Render(verna::ShaderId shader);
    void MoveTowards(float dt, const verna::Vec3f& target);
    verna::Vec3f Direction() const;
    verna::Vec3f HeadPosition() const;
    const auto& Colliders() const { return colliders; }
    bool Hits(const verna::BoundingSphere& sphere) const;
    bool Hits(BufferView<verna::BoundingBox> boxes) const;
    void GrowTail();
    bool HitsItself() const;

   private:
    static constexpr float mov_speed = 2.8f;
    static constexpr float rot_speed = 3.8f;
    float distance;
    verna::Mesh mesh;
    verna::Material material;
    std::vector<verna::Transform> transforms;
    std::vector<verna::BoundingSphere> colliders;
    void Adjust();
};
}  // namespace snake

#endif