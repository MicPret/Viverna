#ifndef VERNA_DIRECTION_LIGHT_SERIALIZER_HPP
#define VERNA_DIRECTION_LIGHT_SERIALIZER_HPP

#include "Vec3fSerializer.hpp"
#include <viverna/graphics/DirectionLight.hpp>

#include <yaml-cpp/yaml.h>

namespace YAML {
template <>
struct convert<verna::DirectionLight> {
    static Node encode(const verna::DirectionLight& rhs);
    static bool decode(const Node& node, verna::DirectionLight& rhs);
};

inline Node convert<verna::DirectionLight>::encode(
    const verna::DirectionLight& rhs) {
    Node node;
    node["direction"] = rhs.direction;
    node["ambient"] = rhs.ambient;
    node["diffuse"] = rhs.diffuse;
    node["specular"] = rhs.specular;
    return node;
}
inline bool convert<verna::DirectionLight>::decode(const Node& node,
                                                   verna::DirectionLight& rhs) {
    if (!node.IsMap())
        return false;
    Node dir = node["direction"];
    Node amb = node["ambient"];
    Node dif = node["diffuse"];
    Node spec = node["specular"];
    if (!dir || !amb || !dif || !spec)
        return false;
    rhs.direction = dir.as<verna::Vec3f>();
    rhs.ambient = amb.as<verna::Vec3f>();
    rhs.diffuse = dif.as<verna::Vec3f>();
    rhs.specular = spec.as<verna::Vec3f>();
    return true;
}

}  // namespace YAML

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const DirectionLight& m);
}  // namespace verna

#endif
