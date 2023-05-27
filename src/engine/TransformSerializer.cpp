#include <viverna/serialization/TransformSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Transform& v) {
    out << YAML::BeginMap;
    out << YAML::Key << "position" << YAML::Value << v.position;
    out << YAML::Key << "rotation" << YAML::Value << v.rotation;
    out << YAML::Key << "scale" << YAML::Value << v.scale;
    out << YAML::EndMap;
    return out;
}
}  // namespace verna
