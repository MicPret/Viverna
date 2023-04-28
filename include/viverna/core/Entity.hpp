#ifndef VERNA_ENTITY_HPP
#define VERNA_ENTITY_HPP

#include "TypeId.hpp"
#include <cstdint>

namespace verna {

using EntityId = uint32_t;
class Family;

struct Entity {
    EntityId id;
    bool Matches(const Family& family) const;
    bool HasComponent(TypeId component_type) const;
};

constexpr bool operator==(Entity a, Entity b) {
    return a.id == b.id;
}

}  // namespace verna

#endif
