#ifndef VERNA_TEXTURE_SERIALIZER_HPP
#define VERNA_TEXTURE_SERIALIZER_HPP

#include <viverna/core/Scene.hpp>
#include <viverna/graphics/Texture.hpp>

#include <yaml-cpp/yaml.h>

namespace verna {
struct TextureSerializer {
    TextureManager& texture_manager;
    TextureId texture;
    constexpr TextureSerializer(TextureManager& tex_man, TextureId texture_) :
        texture_manager(tex_man), texture(texture_) {}
    constexpr TextureSerializer(TextureManager& tex_man) :
        texture_manager(tex_man) {}
};
YAML::Emitter& operator<<(YAML::Emitter& out, const TextureSerializer& v);
}  // namespace verna

namespace YAML {
template <>
struct convert<verna::TextureSerializer> {
    static Node encode(const verna::TextureSerializer& rhs);
    static bool decode(const Node& node, verna::TextureSerializer& rhs);
};

inline Node convert<verna::TextureSerializer>::encode(
    const verna::TextureSerializer& rhs) {
    if (!rhs.texture.IsValid())
        return Node(0);
    auto path = rhs.texture_manager.GetTexturePath(rhs.texture);
    if (!path.empty())
        return Node(path.string());
    auto color = rhs.texture_manager.GetTextureColor(rhs.texture, 0, 0);
    Node node;
    node.push_back(static_cast<unsigned>(color.red));
    node.push_back(static_cast<unsigned>(color.green));
    node.push_back(static_cast<unsigned>(color.blue));
    node.push_back(static_cast<unsigned>(color.alpha));
    return node;
}
inline bool convert<verna::TextureSerializer>::decode(
    const Node& node,
    verna::TextureSerializer& rhs) {
    if (node.as<int>(1) == 0) {
        rhs.texture = {};
        return true;
    }
    verna::TextureLoadConfig config(verna::TextureLoadConfig::KeepInCpuMemory);
    if (node.IsSequence()) {
        unsigned r, g, b, a;
        r = node[0].as<unsigned>();
        g = node[1].as<unsigned>();
        b = node[2].as<unsigned>();
        a = node[3].as<unsigned>();
        Color4u8 col(static_cast<uint8_t>(r), static_cast<uint8_t>(g),
                     static_cast<uint8_t>(b), static_cast<uint8_t>(a));
        rhs.texture = rhs.texture_manager.LoadTextureFromColor(col, config);
        return rhs.texture.IsValid();
    }
    auto name = node.as<std::string>(std::string());
    if (name.empty())
        return false;
    rhs.texture = rhs.texture_manager.LoadTexture(name, config);
    return rhs.texture.IsValid();
}

}  // namespace YAML

#endif
