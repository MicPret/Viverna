#include <viverna/serialization/ShaderSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const ShaderId& v) {
    return out << verna::GetShaderName(v);
}
}  // namespace verna
