#ifndef VERNA_MATERIAL_SERIALIZER_HPP
#define VERNA_MATERIAL_SERIALIZER_HPP

#include "TextureSerializer.hpp"
#include <viverna/graphics/Material.hpp>

#include <yaml-cpp/yaml.h>

#include <cmath>
#include <string>

namespace YAML {
template <>
struct convert<verna::Material> {
    static Node encode(const verna::Material& rhs);
    static bool decode(const Node& node, verna::Material& rhs);
};

inline Node convert<verna::Material>::encode(const verna::Material& rhs) {
    Node node;
    for (auto t : rhs.textures)
        node["textures"].push_back(t);
    node["parameters"] = rhs.parameters;
    return node;
}
inline bool convert<verna::Material>::decode(const Node& node,
                                             verna::Material& rhs) {
    if (!node.IsMap())
        return false;
    const Node& tex_node = node["textures"];
    const Node& par_node = node["parameters"];
    if (!tex_node.IsSequence() || !par_node.IsSequence())
        return false;
    size_t size = std::min(tex_node.size(), rhs.textures.size());
    for (size_t i = 0; i < size; i++)
        rhs.textures[i] = tex_node[i].as<verna::TextureId>(verna::TextureId());
    size = std::min(par_node.size(), rhs.parameters.size());
    for (size_t i = 0; i < size; i++)
        rhs.parameters[i] = par_node[i].as<float>();
    return true;
}

}  // namespace YAML

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Material& m);
}  // namespace verna

#endif
