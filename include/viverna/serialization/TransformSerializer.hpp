#ifndef VERNA_TRANSFORM_SERIALIZER_HPP
#define VERNA_TRANSFORM_SERIALIZER_HPP

#include "Vec3fSerializer.hpp"
#include "QuaternionSerializer.hpp"
#include <viverna/core/Transform.hpp>

#include <yaml-cpp/yaml.h>

namespace YAML {
template <>
struct convert<verna::Transform> {
    static Node encode(const verna::Transform& rhs);
    static bool decode(const Node& node, verna::Transform& rhs);
};

inline Node convert<verna::Transform>::encode(const verna::Transform& rhs) {
    Node node;
    node["position"] = rhs.position;
    node["rotation"] = rhs.rotation;
    node["scale"] = rhs.scale;
    return node;
}
inline bool convert<verna::Transform>::decode(const Node& node,
                                              verna::Transform& rhs) {
    if (!node.IsMap())
        return false;
    YAML::Node position = node["position"];
    YAML::Node rotation = node["rotation"];
    YAML::Node scale = node["scale"];
    if (!position || !rotation || !scale)
        return false;
    rhs.position = position.as<verna::Vec3f>();
    rhs.rotation = rotation.as<verna::Quaternion>();
    rhs.scale = scale.as<verna::Vec3f>();
    return true;
}

}  // namespace YAML

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Transform& v);
}  // namespace verna

#endif
