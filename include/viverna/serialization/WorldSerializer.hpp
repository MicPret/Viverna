#ifndef VERNA_WORLD_SERIALIZER_HPP
#define VERNA_WORLD_SERIALIZER_HPP

#include <viverna/ecs/World.hpp>

#include <yaml-cpp/yaml.h>
namespace verna {
YAML::Emitter& SerializeEntities(YAML::Emitter& emitter,
                                 const World& world,
                                 const std::vector<Entity>& entities);
bool DeserializeEntities(const YAML::Node& node,
                         World& out_world,
                         std::vector<Entity>& out_entities);
}  // namespace verna

#endif
