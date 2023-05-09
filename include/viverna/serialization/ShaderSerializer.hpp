#ifndef VERNA_SHADER_SERIALIZER_HPP
#define VERNA_SHADER_SERIALIZER_HPP

#include <viverna/graphics/Shader.hpp>

#include <yaml-cpp/yaml.h>

namespace YAML {
template <>
struct convert<verna::ShaderId> {
    static Node encode(const verna::ShaderId& rhs);
    static bool decode(const Node& node, verna::ShaderId& rhs);
};

inline Node convert<verna::ShaderId>::encode(const verna::ShaderId& rhs) {
    return Node(verna::GetShaderName(rhs));
}
inline bool convert<verna::ShaderId>::decode(const Node& node,
                                             verna::ShaderId& rhs) {
    auto name = node.as<std::string>(std::string());
    if (name.empty())
        return false;
    rhs = verna::LoadShader(name);
    return true;
}

}  // namespace YAML

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const ShaderId& v);
}  // namespace verna

#endif
