#ifndef VERNA_TEXTURE_SERIALIZER_HPP
#define VERNA_TEXTURE_SERIALIZER_HPP

#include <viverna/graphics/Texture.hpp>

#include <yaml-cpp/yaml.h>

namespace YAML {
template <>
struct convert<verna::TextureId> {
    static Node encode(const verna::TextureId& rhs);
    static bool decode(const Node& node, verna::TextureId& rhs);
};

inline Node convert<verna::TextureId>::encode(const verna::TextureId& rhs) {
    if (!rhs.IsValid())
        return Node(0);
    auto path = verna::GetTexturePath(rhs);
    if (!path.empty())
        return Node(path.string());
    auto color = verna::GetTextureColor(rhs, 1, 1, 0, 0);
    Node node;
    node.push_back(color[0]);
    node.push_back(color[1]);
    node.push_back(color[2]);
    node.push_back(color[3]);
    return node;
}
inline bool convert<verna::TextureId>::decode(const Node& node,
                                              verna::TextureId& rhs) {
    if (node.as<int>(1) == 0) {
        rhs = verna::TextureId();
        return true;
    }
    if (node.IsSequence()) {
        uint8_t r, g, b, a;
        r = node[0].as<uint8_t>();
        g = node[1].as<uint8_t>();
        b = node[2].as<uint8_t>();
        a = node[3].as<uint8_t>();
        rhs = verna::LoadTextureFromColor(r, g, b, a);
        return rhs.IsValid();
    }
    auto name = node.as<std::string>(std::string());
    if (name.empty())
        return false;
    rhs = verna::LoadTexture(name);
    return rhs.IsValid();
}

}  // namespace YAML

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const TextureId& v);
}  // namespace verna

#endif
