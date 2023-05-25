#include <viverna/serialization/ShaderSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const ShaderSerializer& v) {
    return out << v.shader_manager.GetShaderName(v.shader);
}
}  // namespace verna
