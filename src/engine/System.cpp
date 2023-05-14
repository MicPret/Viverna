#include <viverna/ecs/System.hpp>

#include <set>
#include <utility>

namespace verna {

SystemId System::next_id = 0;

System::System(const Family& family_, SystemUpdate update_func) :
    id(++next_id), update{update_func}, family(family_) {}
System::System(Family&& family_, SystemUpdate update_func) :
    id(++next_id), update{update_func}, family(std::move(family_)) {}

SystemId System::Id() const {
    return id;
}
const Family& System::GetFamily() const {
    return family;
}

void System::Notify(EntityEvent event) {
    entity_queue.push_back(event);
}

void System::ReassignEntities(std::vector<Entity>&& new_entities) {
    entities = std::move(new_entities);
}

void System::Run(World& world) {
    ResolveEvents();
    update(world, entities);
}

bool System::operator==(const System& other) const {
    return id == other.id;
}

void System::ResolveEvents() {
    if (entity_queue.empty())
        return;
    std::set<Entity> all;
    for (size_t i = 0; i < entities.size(); i++)
        all.insert(entities[i]);
    std::vector<Entity> remove;
    remove.reserve(entity_queue.size());
    for (size_t i = 0; i < entity_queue.size(); i++) {
        switch (entity_queue[i].event) {
            case EntityEvent::ADD:
                all.insert(entity_queue[i].entity);
                break;
            case EntityEvent::REMOVE:
                remove.push_back(entity_queue[i].entity);
                break;
            case EntityEvent::CLEAR_DATA:
                all.clear();
                remove.clear();
                break;
        }
    }
    for (size_t i = 0; i < remove.size(); i++)
        all.erase(remove[i]);
    ReassignEntities(std::vector(all.begin(), all.end()));
    entity_queue.clear();
}

}  // namespace verna
