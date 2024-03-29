#include <viverna/ecs/World.hpp>

#include <algorithm>
#include <iterator>
#include <set>

namespace verna {

void World::ClearData() {
    for (auto& b : buffers)
        b->Clear();
    for (auto& s : systems)
        s.Notify(EntityEvent(Entity(), EntityEvent::CLEAR_DATA));
    next_id = 0;
}

void World::ClearSystems() {
    systems.clear();
}

void World::ClearAll() {
    ClearData();
    ClearSystems();
}

void World::AddSystem(const System& system) {
    systems.push_back(system);
    std::set<Entity> set;
    const auto& family = system.GetFamily();
    for (TypeId type : family) {
        uint32_t index;
        if (!component_types.GetIndex(type, index))
            return;
        auto entities = buffers[index]->GetEntities();
        for (size_t i = 0; i < entities.size(); i++)
            set.insert(entities[i]);
    }
    systems.back().ReassignEntities(std::vector(set.begin(), set.end()));
}

SystemId World::AddSystem(const Family& family, SystemUpdate update_func) {
    System& system = systems.emplace_back(family, update_func);
    std::set<Entity> set;
    for (TypeId type : family) {
        uint32_t index;
        if (!component_types.GetIndex(type, index))
            return System::InvalidId();
        auto entities = buffers[index]->GetEntities();
        for (size_t i = 0; i < entities.size(); i++)
            set.insert(entities[i]);
    }
    system.ReassignEntities(std::vector(set.begin(), set.end()));
    return system.Id();
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
    delta_time = dt;
    for (System& s : systems)
        s.Run(*this);
}

bool World::HasComponent(Entity e, TypeId comp_type) const {
    uint32_t i;
    if (!component_types.GetIndex(comp_type, i))
        return false;
    return buffers[i]->Contains(e);
}

bool World::Matches(Entity e, const Family& family) const {
    for (TypeId type : family)
        if (!HasComponent(e, type))
            return false;
    return true;
}

DeltaTime<float, Seconds> World::GetDeltaTime() const {
    return delta_time;
}

void World::RemoveEntity(Entity e) {
    EntityEvent event(e, EntityEvent::REMOVE);
    for (System& s : systems)
        s.Notify(event);
}

std::vector<Entity> World::GetEntitiesWithComponent(TypeId comp_type) const {
    SparseSet<TypeId>::index_t i;
    if (component_types.GetIndex(comp_type, i)) {
        const auto& buffer = buffers[i];
        return buffer->GetEntities();
    }
    return {};
}

std::vector<Entity> World::GetEntitiesInFamily(const Family& family) const {
    if (family.Empty())
        return {};
    constexpr auto entity_comp = [](Entity a, Entity b) { return a.id < b.id; };
    auto result = GetEntitiesWithComponent(*family.begin());
    for (auto it = family.begin() + 1; it != family.end(); ++it) {
        auto entities = GetEntitiesWithComponent(*it);
        std::vector<Entity> temp;
        std::set_intersection(result.begin(), result.end(), entities.begin(),
                              entities.end(), std::back_inserter(temp),
                              entity_comp);
        result = std::move(temp);
    }
    return result;
}

}  // namespace verna
