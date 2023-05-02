#ifndef VERNA_SYSTEM_HPP
#define VERNA_SYSTEM_HPP

#include "Entity.hpp"
#include "EntityEvent.hpp"
#include "Family.hpp"
#include <viverna/core/Time.hpp>
#include <vector>

namespace verna {

using SystemUpdate = void (*)(std::vector<Entity>& entities,
                              DeltaTime<float, Seconds> dt);
using SystemId = uint32_t;

class System {
   public:
    System(const Family& family_, SystemUpdate update_func);
    System(Family&& family_, SystemUpdate update_func);
    SystemId Id() const;
    const Family& GetFamily() const;
    void Run(DeltaTime<float, Seconds> dt);
    void Notify(EntityEvent event);
    void ReassignEntities(std::vector<Entity>&& new_entities);
    bool operator==(const System& other) const;
    template <typename... T>
    static System FromFamilyOf(SystemUpdate update_func) {
        return System(Family::From<T...>(), update_func);
    }
    static constexpr SystemId InvalidId() { return 0; }

   private:
    SystemId id;
    SystemUpdate update;
    Family family;
    std::vector<Entity> entities;
    std::vector<EntityEvent> entity_queue;
    void ResolveEvents();
    static SystemId next_id;
};

}  // namespace verna

#endif
