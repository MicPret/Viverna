#include <viverna/serialization/TextureSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const TextureId& v) {
    if (!v.IsValid()) {
        out << 0;
        return out;
    }
    auto path = verna::GetTexturePath(v);
    if (!path.empty())
        out << path.string();
    else {
        auto color = verna::GetTextureColor(v, 1, 1, 0, 0);
        out << YAML::Flow << YAML::BeginSeq;
        out << static_cast<unsigned>(color[0])
            << static_cast<unsigned>(color[1])
            << static_cast<unsigned>(color[2])
            << static_cast<unsigned>(color[3]);
        out << YAML::EndSeq;
    }
    return out;
}
}  // namespace verna
