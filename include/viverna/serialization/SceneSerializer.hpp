#ifndef VERNA_SCENE_SERIALIZER_HPP
#define VERNA_SCENE_SERIALIZER_HPP

#include "CameraSerializer.hpp"
#include "DirectionLightSerializer.hpp"
#include "WorldSerializer.hpp"
#include <viverna/core/Scene.hpp>
#include <viverna/ecs/Entity.hpp>

#include <yaml-cpp/yaml.h>

#include <vector>

namespace YAML {
template <>
struct convert<verna::Scene> {
    static Node encode(const verna::Scene& rhs);
    static bool decode(const Node& node, verna::Scene& rhs);
};

inline Node convert<verna::Scene>::encode(const verna::Scene& rhs) {
    Node node;
    node["camera"] = rhs.camera;
    node["direction_light"] = rhs.direction_light;
    return node;
}
inline bool convert<verna::Scene>::decode(const Node& node, verna::Scene& rhs) {
    if (!node.IsMap()) {
        VERNA_LOGE("Scene node is not a map!");
        return false;
    }
    Node camera = node["camera"];
    if (!camera) {
        VERNA_LOGE("Camera node not found!");
        return false;
    }
    Node dirlight = node["direction_light"];
    if (!dirlight) {
        VERNA_LOGE("Direction light node not found!");
        return false;
    }
    rhs.camera = camera.as<verna::Camera>();
    rhs.direction_light = dirlight.as<verna::DirectionLight>();
    return true;
}

}  // namespace YAML

namespace verna {
YAML::Emitter& SerializeScene(YAML::Emitter& emitter, Scene& scene);
bool DeserializeScene(const YAML::Node& node,
                      Scene& out_scene,
                      std::vector<Entity>& out_entities);
}  // namespace verna

#endif
