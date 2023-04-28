#ifndef VERNA_SYSTEM_HPP
#define VERNA_SYSTEM_HPP

#include "Entity.hpp"
#include "Family.hpp"
#include "Time.hpp"
#include <vector>

namespace verna {

using SystemUpdate = void (*)(std::vector<Entity>& entities,
                              DeltaTime<float, Seconds> dt);

class System {
   public:
    System(const Family& family_, SystemUpdate update_func);
    System(Family&& family_, SystemUpdate update_func);
    void NotifyNewEntity(Entity e);
    void NotifyRemovedEntity(Entity e);

   private:
    Family family;
    SystemUpdate update;
    std::vector<Entity> entities;
};
}  // namespace verna

#endif
