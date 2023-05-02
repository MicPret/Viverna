#include <viverna/ecs/World.hpp>

#include <set>

namespace verna {

void World::Clear() {
    for (auto& b : buffers)
        b->Clear();
    systems.clear();
    next_id = 0;
}

void World::AddSystem(const System& system) {
    systems.push_back(system);
    std::set<Entity> set;
    const auto& family = system.GetFamily();
    for (TypeId type : family) {
        uint32_t index;
        if (!component_types.GetIndex(type, index))
            return;
        const auto& entity_ids = buffers[index]->GetEntityIds();
        for (size_t i = 0; i < entity_ids.size(); i++)
            set.insert(Entity(entity_ids[i]));
    }
    systems.back().ReassignEntities(std::vector(set.begin(), set.end()));
}

SystemId World::AddSystem(const Family& family, SystemUpdate update_func) {
    System& system = systems.emplace_back(family, update_func);
    std::set<Entity> set;
    const auto& family = system.GetFamily();
    for (TypeId type : family) {
        uint32_t index;
        if (!component_types.GetIndex(type, index))
            return System::InvalidId();
        const auto& entity_ids = buffers[index]->GetEntityIds();
        for (size_t i = 0; i < entity_ids.size(); i++)
            set.insert(Entity(entity_ids[i]));
    }
    system.ReassignEntities(std::vector(set.begin(), set.end()));
}

void World::RemoveSystem(SystemId system_id) {
    for (size_t i = 0; i < systems.size(); i++) {
        if (systems[i].Id() == system_id) {
            systems.erase(systems.begin() + i);
            return;
        }
    }
}

void World::RunSystems(DeltaTime<float, Seconds> dt) {
    for (System& s : systems)
        s.Run(dt);
}

bool World::HasComponent(Entity e, TypeId comp_type) const {
    uint32_t i;
    if (!component_types.GetIndex(comp_type, i))
        return false;
    return buffers[i]->Contains(e);
}

bool World::Matches(Entity e, const Family& family) const {
    uint32_t i;
    for (TypeId type : family)
        if (!HasComponent(e, type))
            return false;
    return true;
}

}  // namespace verna
