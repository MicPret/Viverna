#ifndef VERNA_VIV_SERIALIZER_HPP
#define VERNA_VIV_SERIALIZER_HPP

#include <viverna/core/Scene.hpp>
#include <viverna/ecs/World.hpp>

#include <yaml-cpp/yaml.h>

#include <string_view>
#include <vector>

namespace verna {
YAML::Emitter& Serialize(YAML::Emitter& emitter,
                         const Scene& scene,
                         const World& world,
                         const std::vector<Entity>& entities);
bool Deserialize(const YAML::Node& node,
                 Scene& out_scene,
                 World& out_world,
                 std::vector<Entity>& out_entities);

bool LoadViv(std::string_view viv_file,
             Scene& out_scene,
             World& out_world,
             std::vector<Entity>& out_entities);
}  // namespace verna

#endif
