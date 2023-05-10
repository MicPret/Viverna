#ifndef VERNA_QUATERNION_SERIALIZER_HPP
#define VERNA_QUATERNION_SERIALIZER_HPP

#include <viverna/maths/Quaternion.hpp>

#include <yaml-cpp/yaml.h>

namespace YAML {
template <>
struct convert<verna::Quaternion> {
    static Node encode(const verna::Quaternion& rhs);
    static bool decode(const Node& node, verna::Quaternion& rhs);
};

inline Node convert<verna::Quaternion>::encode(const verna::Quaternion& rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    node.push_back(rhs.w);
    return node;
}
inline bool convert<verna::Quaternion>::decode(const Node& node,
                                               verna::Quaternion& rhs) {
    if (!node.IsSequence() || node.size() != 4)
        return false;
    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    rhs.z = node[2].as<float>();
    rhs.w = node[3].as<float>();
    return true;
}

}  // namespace YAML

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Quaternion& v);
}  // namespace verna

#endif
