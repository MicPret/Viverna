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
        auto color = verna::GetTextureColor(v, 0, 0);
        out << YAML::Flow << YAML::BeginSeq;
        out << static_cast<unsigned>(color.red)
            << static_cast<unsigned>(color.green)
            << static_cast<unsigned>(color.blue)
            << static_cast<unsigned>(color.alpha);
        out << YAML::EndSeq;
    }
    return out;
}
}  // namespace verna
