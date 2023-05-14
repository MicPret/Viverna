#ifndef VERNA_SCENE_SERIALIZER_HPP
#define VERNA_SCENE_SERIALIZER_HPP

#include "CameraSerializer.hpp"
#include "DirectionLightSerializer.hpp"
#include <viverna/core/Scene.hpp>

#include <yaml-cpp/yaml.h>

namespace YAML {
template <>
struct convert<verna::Scene> {
    static Node encode(const verna::Scene& rhs);
    static bool decode(const Node& node, verna::Scene& rhs);
};

inline Node convert<verna::Scene>::encode(const verna::Scene& rhs) {
    Node node;
    node["camera"] = rhs.GetCamera();
    node["direction_light"] = rhs.GetDirectionLight();
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
    rhs.GetCamera() = camera.as<verna::Camera>();
    rhs.GetDirectionLight() = dirlight.as<verna::DirectionLight>();
    return true;
}

}  // namespace YAML

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Scene& m);
}  // namespace verna

#endif
