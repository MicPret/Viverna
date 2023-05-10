#include <viverna/serialization/VivSerializer.hpp>
#include <viverna/serialization/SceneSerializer.hpp>

namespace verna {

YAML::Emitter& Serialize(YAML::Emitter& emitter,
                         const Scene& scene,
                         const World& world,
                         const std::vector<Entity>& entities) {
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "scene" << YAML::Value << scene;
    emitter << YAML::Key << "entities" << YAML::Value;
    SerializeEntities(emitter, world, entities);
    emitter << YAML::EndMap;
    return emitter;
}
}  // namespace verna
