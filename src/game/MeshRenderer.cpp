#include <game/systems/MeshRenderer.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/Shader.hpp>

namespace editor {

struct RenderInfo {
    verna::Mesh mesh;
    verna::Material material;
    verna::ShaderId shader;
    verna::Transform transform;
};

void Render(verna::World& world, std::vector<verna::Entity>& entities) {
    RenderInfo r;
    for (auto e : entities) {
        world.GetComponents(e, r.mesh, r.material, r.shader, r.transform);
        verna::Render(r.mesh, r.material, r.transform, r.shader);
    }
}
}  // namespace editor
