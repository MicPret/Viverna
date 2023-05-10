#ifndef VERNA_VIV_SERIALIZER_HPP
#define VERNA_VIV_SERIALIZER_HPP

#include "WorldSerializer.hpp"

#include <viverna/core/Scene.hpp>

namespace verna {
YAML::Emitter& Serialize(YAML::Emitter& emitter,
                         const Scene& scene,
                         const World& world,
                         const std::vector<Entity>& entities);
}  // namespace verna

#endif
