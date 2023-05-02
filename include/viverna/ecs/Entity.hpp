#ifndef VERNA_ENTITY_HPP
#define VERNA_ENTITY_HPP

#include <viverna/core/TypeId.hpp>
#include <cstdint>

namespace verna {

using EntityId = uint32_t;
class Family;

struct Entity {
    EntityId id;
    constexpr Entity() : id(0) {}
    explicit constexpr Entity(EntityId id_) : id(id_) {}
};

constexpr bool operator==(Entity a, Entity b) {
    return a.id == b.id;
}

}  // namespace verna

#endif
