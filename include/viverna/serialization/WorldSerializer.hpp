#ifndef VERNA_WORLD_SERIALIZER_HPP
#define VERNA_WORLD_SERIALIZER_HPP

#include <viverna/ecs/World.hpp>
#include <viverna/graphics/ShaderManager.hpp>
#include <viverna/graphics/TextureManager.hpp>

#include <yaml-cpp/yaml.h>
namespace verna {
YAML::Emitter& SerializeWorld(YAML::Emitter& emitter,
                              ShaderManager& shader_man,
                              TextureManager& texture_man,
                              const World& world);
bool DeserializeWorld(const YAML::Node& node,
                      ShaderManager& shader_man,
                      TextureManager& texture_man,
                      World& out_world,
                      std::vector<Entity>& out_entities);
}  // namespace verna

#endif
