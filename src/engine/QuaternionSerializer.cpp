#include <viverna/serialization/QuaternionSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const Quaternion& v) {
    out << YAML::Flow << YAML::BeginSeq << v.x << v.y << v.z << v.w
        << YAML::EndSeq;
    return out;
}
}  // namespace verna
