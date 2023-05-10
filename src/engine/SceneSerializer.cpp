#include <viverna/serialization/SceneSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Scene& m) {
    out << YAML::BeginMap;
    out << YAML::Key << "camera" << YAML::Value << m.GetCamera();
    out << YAML::Key << "direction_light" << YAML::Value
        << m.GetDirectionLight();
    out << YAML::EndMap;
    return out;
}
}  // namespace verna
