#ifndef VERNA_VEC3F_SERIALIZER_HPP
#define VERNA_VEC3F_SERIALIZER_HPP

#include <viverna/maths/Vec3f.hpp>

#include <yaml-cpp/yaml.h>

namespace YAML {
template <>
struct convert<verna::Vec3f> {
    static Node encode(const verna::Vec3f& rhs);
    static bool decode(const Node& node, verna::Vec3f& rhs);
};

inline Node convert<verna::Vec3f>::encode(const verna::Vec3f& rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    return node;
}
inline bool convert<verna::Vec3f>::decode(const Node& node, verna::Vec3f& rhs) {
    if (!node.IsSequence() || node.size() != 3)
        return false;
    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    rhs.z = node[2].as<float>();
    return true;
}

}  // namespace YAML

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Vec3f& v);
}  // namespace verna

#endif
