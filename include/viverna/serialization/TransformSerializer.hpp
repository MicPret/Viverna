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
    node.push_back(rhs.position);
    node.push_back(rhs.rotation);
    node.push_back(rhs.scale);
    return node;
}
inline bool convert<verna::Transform>::decode(const Node& node,
                                              verna::Transform& rhs) {
    if (!node.IsSequence() || node.size() != 3)
        return false;
    rhs.position = node[0].as<verna::Vec3f>();
    rhs.rotation = node[1].as<verna::Quaternion>();
    rhs.scale = node[2].as<verna::Vec3f>();
    return true;
}

}  // namespace YAML

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Transform& v);
}  // namespace verna

#endif
