#ifndef VERNA_ENTITY_EVENT_HPP
#define VERNA_ENTITY_EVENT_HPP

#include "Entity.hpp"

namespace verna {
struct EntityEvent {
    using event_t = uint8_t;
    Entity entity;
    event_t event;
    constexpr EntityEvent(Entity e, event_t ev) : entity(e), event(ev) {}
    static constexpr event_t ADD = 0;
    static constexpr event_t REMOVE = 1;
};
}  // namespace verna

#endif