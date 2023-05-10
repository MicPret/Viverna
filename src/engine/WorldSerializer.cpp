#include <viverna/serialization/WorldSerializer.hpp>
#include <viverna/ecs/EntityName.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/serialization/MaterialSerializer.hpp>
#include <viverna/serialization/ShaderSerializer.hpp>
#include <viverna/serialization/TransformSerializer.hpp>

namespace verna {
YAML::Emitter& SerializeEntities(YAML::Emitter& emitter,
                                 const World& world,
                                 const std::vector<Entity>& entities) {
    EntityName name;
    Material material;
    Mesh mesh;
    ShaderId shader;
    Transform transform;
    emitter << YAML::BeginMap;
    for (size_t i = 0; i < entities.size(); i++) {
        world.GetComponents(entities[i], name, material, mesh, shader,
                            transform);
        emitter << YAML::Key << name.str;
        emitter << YAML::Value << YAML::BeginMap;
        emitter << YAML::Key << "material" << YAML::Value << material;
        emitter << YAML::Key << "mesh" << YAML::Value << GetMeshName(mesh.id);
        emitter << YAML::Key << "shader" << YAML::Value << shader;
        emitter << YAML::Key << "transform" << YAML::Value << transform;
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndMap;
    return emitter;
}
}  // namespace verna
