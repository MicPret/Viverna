#ifndef VERNA_CAMERA_SERIALIZER_HPP
#define VERNA_CAMERA_SERIALIZER_HPP

#include "QuaternionSerializer.hpp"
#include "Vec3fSerializer.hpp"
#include <viverna/graphics/Camera.hpp>

#include <yaml-cpp/yaml.h>

namespace YAML {
template <>
struct convert<verna::Camera> {
    static Node encode(const verna::Camera& rhs);
    static bool decode(const Node& node, verna::Camera& rhs);
};

inline Node convert<verna::Camera>::encode(const verna::Camera& rhs) {
    Node node;
    node["position"] = rhs.position;
    node["rotation"] = rhs.rotation;
    node["fov"] = rhs.fovy;
    node["aspect_ratio"] = rhs.aspect_ratio;
    node["near"] = rhs.near_plane;
    node["far"] = rhs.far_plane;
    return node;
}
inline bool convert<verna::Camera>::decode(const Node& node,
                                           verna::Camera& rhs) {
    if (!node.IsMap())
        return false;
    Node position = node["position"];
    Node rotation = node["rotation"];
    Node fov = node["fov"];
    Node aspect = node["aspect_ratio"];
    Node near = node["near"];
    Node far = node["far"];
    bool failure = !position || !rotation || !fov || !aspect || !near || !far;
    if (failure)
        return false;
    rhs.position = position.as<verna::Vec3f>();
    rhs.rotation = rotation.as<verna::Quaternion>();
    rhs.fovy = fov.as<float>();
    rhs.aspect_ratio = aspect.as<float>();
    rhs.near_plane = near.as<float>();
    rhs.far_plane = far.as<float>();
    return true;
}
}  // namespace YAML
namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Camera& m);
}

#endif
