#include <viverna/serialization/VivSerializer.hpp>
#include <viverna/core/Assets.hpp>
#include <viverna/serialization/SceneSerializer.hpp>
#include <viverna/serialization/WorldSerializer.hpp>

#include <filesystem>

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

bool Deserialize(const YAML::Node& node,
                 Scene& out_scene,
                 World& out_world,
                 std::vector<Entity>& out_entities) {
    YAML::Node scene_node = node["scene"];
    if (!scene_node) {
        VERNA_LOGE("Scene node not found!");
        return false;
    }
    YAML::Node entities_node = node["entities"];
    if (!entities_node) {
        VERNA_LOGE("Entities node not found!");
        return false;
    }
    Scene scene = scene_node.as<Scene>(Scene());
    scene.Setup();
    Scene::GetActive() = scene;
    return DeserializeEntities(node, out_world, out_entities);
}

bool LoadViv(std::string_view viv_file,
             Scene& out_scene,
             World& out_world,
             std::vector<Entity>& out_entities) {
    std::filesystem::path folder = "scenes";
    auto path = folder / viv_file;
    VERNA_LOGI("Loading " + path.string());
    auto raw = LoadRawAsset(path);
    auto yaml_string = std::string(raw.data(), raw.size());
    YAML::Node node = YAML::Load(yaml_string);
    return Deserialize(node, out_scene, out_world, out_entities);
}
}  // namespace verna
