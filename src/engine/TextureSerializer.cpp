#include <viverna/serialization/TextureSerializer.hpp>

namespace verna {
YAML::Emitter& operator<<(YAML::Emitter& out, const TextureSerializer& v) {
    if (!v.texture.IsValid()) {
        out << 0;
        return out;
    }
    auto path = v.texture_manager.GetTexturePath(v.texture);
    if (!path.empty())
        out << path.string();
    else {
        auto color = v.texture_manager.GetTextureColor(v.texture, 0, 0);
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
