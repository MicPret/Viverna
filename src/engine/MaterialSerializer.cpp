#include <viverna/serialization/MaterialSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Material& m) {
    out << YAML::BeginMap;
    out << YAML::Key << "textures";
    out << YAML::Value << YAML::BeginSeq;
    for (auto t : m.textures) {
        if (t.IsValid()) {
            auto p = verna::GetTexturePath(t);
            // TODO get color
            out << (p.empty() ? std::string("unknown_texture") : p.string());
        } else {
            out << "0";
        }
    }
    out << YAML::EndSeq;
    out << YAML::Key << "parameters";
    out << YAML::Value << YAML::BeginSeq;
    for (size_t i = 0; i < m.parameters.size(); i++)
        out << m.parameters[i];
    out << YAML::EndSeq;
    out << YAML::EndMap;
    return out;
}
}  // namespace verna
