#include <viverna/serialization/TransformSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Transform& v) {
    out << YAML::BeginSeq << v.position << v.rotation << v.scale
        << YAML::EndSeq;
    return out;
}
}  // namespace verna
