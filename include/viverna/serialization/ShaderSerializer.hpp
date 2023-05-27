#ifndef VERNA_SHADER_SERIALIZER_HPP
#define VERNA_SHADER_SERIALIZER_HPP

#include <viverna/graphics/ShaderManager.hpp>

#include <yaml-cpp/yaml.h>

namespace verna {
struct ShaderSerializer {
    ShaderManager& shader_manager;
    ShaderId shader;
    constexpr ShaderSerializer(ShaderManager& shader_man) :
        shader_manager(shader_man) {}
};
YAML::Emitter& operator<<(YAML::Emitter& out, const ShaderSerializer& v);
}  // namespace verna

namespace YAML {
template <>
struct convert<verna::ShaderSerializer> {
    static Node encode(const verna::ShaderSerializer& rhs);
    static bool decode(const Node& node, verna::ShaderSerializer& rhs);
};

inline Node convert<verna::ShaderSerializer>::encode(
    const verna::ShaderSerializer& rhs) {
    return Node(rhs.shader_manager.GetShaderName(rhs.shader));
}
inline bool convert<verna::ShaderSerializer>::decode(
    const Node& node,
    verna::ShaderSerializer& rhs) {
    auto name = node.as<std::string>(std::string());
    if (name.empty())
        return false;
    rhs.shader = rhs.shader_manager.LoadShader(name);
    return true;
}

}  // namespace YAML

#endif
