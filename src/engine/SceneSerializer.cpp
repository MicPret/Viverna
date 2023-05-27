#include <viverna/serialization/SceneSerializer.hpp>

namespace verna {

YAML::Emitter& SerializeScene(YAML::Emitter& emitter, Scene& scene) {
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "camera" << YAML::Value << scene.camera;
    emitter << YAML::Key << "direction_light" << YAML::Value
            << scene.direction_light;
    emitter << YAML::Key << "entities" << YAML::Value;
    SerializeWorld(emitter, scene.shader_manager, scene.texture_manager,
                   scene.world);
    emitter << YAML::EndMap;
    return emitter;
}
bool DeserializeScene(const YAML::Node& node,
                      Scene& out_scene,
                      std::vector<Entity>& out_entities) {
    YAML::Node cam_node = node["camera"];
    YAML::Node dirlight_node = node["direction_light"];
    YAML::Node world_node = node["entities"];
    if (!cam_node) {
        VERNA_LOGE("Camera node not found!");
        return false;
    }
    if (!dirlight_node) {
        VERNA_LOGE("Direction light node not found!");
        return false;
    }
    if (!world_node) {
        VERNA_LOGE("Entities node not found!");
        return false;
    }
    out_scene.ReleaseResources();
    out_scene.camera = cam_node.as<Camera>(Camera());
    out_scene.direction_light =
        dirlight_node.as<DirectionLight>(DirectionLight());
    return DeserializeWorld(world_node, out_scene.shader_manager,
                            out_scene.texture_manager, out_scene.world,
                            out_entities);
}
}  // namespace verna
