#include <viverna/serialization/MaterialSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const MaterialSerializer& m) {
    out << YAML::BeginMap;
    out << YAML::Key << "textures";
    out << YAML::Value << YAML::BeginSeq;
    TextureSerializer t_serializer(m.texture_manager);
    for (TextureId t : m.material.textures) {
        t_serializer.texture = t;
        out << t_serializer;
    }
    out << YAML::EndSeq;
    out << YAML::Key << "parameters";
    out << YAML::Value << YAML::Flow << YAML::BeginSeq;
    for (size_t i = 0; i < m.material.parameters.size(); i++)
        out << m.material.parameters[i];
    out << YAML::EndSeq;
    out << YAML::EndMap;
    return out;
}
}  // namespace verna
