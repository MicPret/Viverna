#include <viverna/serialization/WorldSerializer.hpp>
#include <viverna/ecs/EntityName.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/serialization/MaterialSerializer.hpp>
#include <viverna/serialization/ShaderSerializer.hpp>
#include <viverna/serialization/TransformSerializer.hpp>

namespace verna {
YAML::Emitter SerializeEntities(const World& world,
                                const std::vector<Entity>& entities) {
    YAML::Emitter result;
    EntityName name;
    Material material;
    Mesh mesh;
    ShaderId shader;
    Transform transform;
    result << YAML::BeginMap;
    for (size_t i = 0; i < entities.size(); i++) {
        world.GetComponents(entities[i], name, material, mesh, shader,
                            transform);
        result << YAML::Key << name.str;
        result << YAML::Value << YAML::BeginMap;
        result << YAML::Key << "material" << YAML::Value << material;
        result << YAML::Key << "mesh" << YAML::Value << GetMeshName(mesh.id);
        result << YAML::Key << "shader" << YAML::Value << shader;
        result << YAML::Key << "transform" << YAML::Value << transform;
        result << YAML::EndMap;
    }
    result << YAML::EndMap;
    return result;
}
}  // namespace verna
