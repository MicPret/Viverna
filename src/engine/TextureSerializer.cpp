#include <viverna/serialization/TextureSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const TextureId& v) {
    auto path = verna::GetTexturePath(v);
    if (!path.empty())
        out << path.string();
    else {
        auto color = verna::GetTextureColor(v, 1, 1, 0, 0);
        out << YAML::Flow << YAML::BeginSeq << color[0] << color[1] << color[2]
            << color[3] << YAML::EndSeq;
    }
    return out;
}
}  // namespace verna
