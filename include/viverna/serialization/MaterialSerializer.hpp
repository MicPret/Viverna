#ifndef VERNA_MATERIAL_SERIALIZER_HPP
#define VERNA_MATERIAL_SERIALIZER_HPP

#include "TextureSerializer.hpp"
#include <viverna/graphics/Material.hpp>

#include <yaml-cpp/yaml.h>

#include <cmath>
#include <string>

namespace verna {
struct MaterialSerializer {
    TextureManager& texture_manager;
    Material material;
    constexpr MaterialSerializer(TextureManager& tex_man, const Material& mat) :
        texture_manager(tex_man), material(mat) {}
    constexpr MaterialSerializer(TextureManager& tex_man) :
        texture_manager(tex_man) {}
};
YAML::Emitter& operator<<(YAML::Emitter& out, const MaterialSerializer& m);
}  // namespace verna

namespace YAML {

template <>
struct convert<verna::MaterialSerializer> {
    static Node encode(const verna::MaterialSerializer& rhs);
    static bool decode(const Node& node, verna::MaterialSerializer& rhs);
};

inline Node convert<verna::MaterialSerializer>::encode(
    const verna::MaterialSerializer& rhs) {
    Node node;
    verna::TextureSerializer tex_serializer(rhs.texture_manager);
    for (verna::TextureId t : rhs.material.textures) {
        tex_serializer.texture = t;
        node["textures"].push_back(tex_serializer);
    }
    node["parameters"] = rhs.material.parameters;
    return node;
}
inline bool convert<verna::MaterialSerializer>::decode(
    const Node& node,
    verna::MaterialSerializer& rhs) {
    if (!node.IsMap())
        return false;
    Node textures_node = node["textures"];
    Node par_node = node["parameters"];
    if (!textures_node.IsSequence() || !par_node.IsSequence())
        return false;
    size_t size = std::min(textures_node.size(), rhs.material.textures.size());
    verna::TextureSerializer tex_serializer(rhs.texture_manager);
    for (size_t i = 0; i < size; i++) {
        Node t_node = textures_node[i];
        if (static_cast<bool>(t_node)
            && convert<verna::TextureSerializer>::decode(t_node,
                                                         tex_serializer))
            rhs.material.textures[i] = tex_serializer.texture;
        else {
            VERNA_LOGE("Failed to decode material textures!");
            return false;
        }
    }
    size = std::min(par_node.size(), rhs.material.parameters.size());
    for (size_t i = 0; i < size; i++)
        rhs.material.parameters[i] = par_node[i].as<float>();
    return true;
}

}  // namespace YAML

#endif
