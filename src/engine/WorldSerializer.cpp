#include <viverna/serialization/WorldSerializer.hpp>
#include <viverna/ecs/EntityName.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/serialization/MaterialSerializer.hpp>
#include <viverna/serialization/ShaderSerializer.hpp>
#include <viverna/serialization/TransformSerializer.hpp>

namespace verna {
YAML::Node SerializeEntities(const World& world,
                             const std::vector<Entity>& entities) {
    YAML::Node result;
    EntityName name;
    Material material;
    Mesh mesh;
    Transform transform;
    ShaderId shader;
    for (size_t i = 0; i < entities.size(); i++) {
        world.GetComponents(entities[i], name, material, mesh, shader,
                            transform);
        result[name.str]["material"] = material;
        result[name.str]["mesh"] = GetMeshName(mesh.id);
        result[name.str]["shader"] = shader;
        result[name.str]["transform"] = transform;
    }
    return result;
}
}  // namespace verna
