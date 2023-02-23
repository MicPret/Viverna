#ifndef VERNA_GAME_WALLS_HPP
#define VERNA_GAME_WALLS_HPP

#include <viverna/core/BoundingBox.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Shader.hpp>

#include <array>

namespace snake {
class Walls {
   public:
    Walls(float distance);
    void Setup();
    void Dispose();
    void Render(verna::ShaderId shader);
    const auto& Colliders() const { return colliders; }

   private:
    verna::Mesh mesh;
    verna::Material bg_material;
    verna::Material wall_material;
    std::array<verna::BoundingBox, 4> colliders;
    std::array<verna::Transform, 5> transforms;
};
}  // namespace snake

#endif