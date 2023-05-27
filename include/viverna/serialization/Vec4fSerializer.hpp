#ifndef VERNA_VEC4F_SERIALIZER_HPP
#define VERNA_VEC4F_SERIALIZER_HPP

#include <viverna/maths/Vec4f.hpp>

#include <yaml-cpp/yaml.h>

namespace YAML {
template <>
struct convert<verna::Vec4f> {
    static Node encode(const verna::Vec4f& rhs);
    static bool decode(const Node& node, verna::Vec4f& rhs);
};

inline Node convert<verna::Vec4f>::encode(const verna::Vec4f& rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    node.push_back(rhs.w);
    return node;
}
inline bool convert<verna::Vec4f>::decode(const Node& node, verna::Vec4f& rhs) {
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
YAML::Emitter& operator<<(YAML::Emitter& out, const Vec4f& v);
}  // namespace verna

#endif
