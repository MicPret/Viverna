#include <viverna/serialization/CameraSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Camera& m) {
    out << YAML::BeginMap;
    out << YAML::Key << "position" << YAML::Value << m.position;
    out << YAML::Key << "rotation" << YAML::Value << m.rotation;
    out << YAML::Key << "fov" << YAML::Value << m.fovy;
    out << YAML::Key << "aspect_ratio" << YAML::Value << m.aspect_ratio;
    out << YAML::Key << "near" << YAML::Value << m.near_plane;
    out << YAML::Key << "far" << YAML::Value << m.far_plane;
    out << YAML::EndMap;
    return out;
}
}  // namespace verna