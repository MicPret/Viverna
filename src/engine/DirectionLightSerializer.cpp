#include <viverna/serialization/DirectionLightSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const DirectionLight& m) {
    out << YAML::BeginMap;
    out << YAML::Key << "direction" << YAML::Value << m.direction;
    out << YAML::Key << "ambient" << YAML::Value << m.ambient;
    out << YAML::Key << "diffuse" << YAML::Value << m.diffuse;
    out << YAML::Key << "specular" << YAML::Value << m.specular;
    out << YAML::EndMap;
    return out;
}
}  // namespace verna
