#include <viverna/core/System.hpp>

#include <utility>

namespace verna {

System::System(const Family& family_, SystemUpdate update_func) :
    family(family_), update{update_func} {}
System::System(Family&& family_, SystemUpdate update_func) :
    family(std::move(family_)), update{update_func} {}

void System::NotifyNewEntity(Entity e) {
    if (e.Matches(family))
        entities.push_back(e);
}
void System::NotifyRemovedEntity(Entity e) {
    if (!e.Matches(family))
        return;
    for (size_t i = 0; i < entities.size(); i++) {
        if (entities[i] == e) {
            entities.erase(entities.begin() + i);
            return;
        }
    }
}

}  // namespace verna
