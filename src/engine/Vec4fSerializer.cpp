#include <viverna/serialization/Vec4fSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Vec4f& v) {
    out << YAML::Flow << YAML::BeginSeq << v.x << v.y << v.z << v.w
        << YAML::EndSeq;
    return out;
}
}  // namespace verna
