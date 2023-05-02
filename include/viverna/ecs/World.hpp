#ifndef VERNA_WORLD_HPP
#define VERNA_WORLD_HPP

#include "ComponentBuffer.hpp"
#include "Entity.hpp"
#include "System.hpp"
#include <viverna/core/Time.hpp>
#include <viverna/data/SparseSet.hpp>
#include <memory>
#include <vector>

namespace verna {
class World {
   public:
    bool HasComponent(Entity e, TypeId comp_type) const;
    bool Matches(Entity e, const Family& family) const;
    void AddSystem(const System& system);
    SystemId AddSystem(const Family& family, SystemUpdate update_func);
    void RemoveSystem(SystemId system_id);
    void Clear();
    void RunSystems(DeltaTime<float, Seconds> dt);
    template <typename... Comps>
    Entity NewEntity();
    template <typename C>
    C GetComponent(Entity e) const;
    template <typename... Comps>
    void GetComponents(Entity e, Comps&... comps) const;
    template <typename C>
    void SetComponent(Entity e, const C& component);
    template <typename... Comps>
    void SetComponents(Entity e, const Comps&... comps) const;
    template <typename C>
    bool HasComponent(Entity e) const;
    template <typename C>
    auto& GetComponentArray() {
        return GetComponentBuffer<C>().GetComponents();
    }

   private:
    SparseSet<TypeId> component_types;
    std::vector<std::unique_ptr<BaseComponentBuffer>> buffers;
    std::vector<System> systems;
    EntityId next_id = 0;
    template <typename T>
    ComponentBuffer<T>& GetComponentBuffer() {
        TypeId type = GetTypeId<T>();
        SparseSet<TypeId>::index_t i;
        if (component_types.GetIndex(type, i)) {
            auto ptr = static_cast<ComponentBuffer<T>*>(buffers[i].get());
            return *ptr;
        } else {
            component_types.Add(type);
            buffers.push_back(std::make_unique<ComponentBuffer>());
            return *buffers.back();
        }
    }
};

template <typename... Comps>
Entity World::NewEntity() {
    Entity e(++next_id);
    SetComponents<Comps...>(e, Comps());
    for (auto& s : systems)
        if (Matches(e, s.GetFamily()))
            s.Notify(EntityEvent(e, EntityEvent::ADD));
    return e;
}

template <typename C>
C World::GetComponent(Entity e) const {
    return GetComponentBuffer<C>().GetComponent(e);
}

template <typename... Comps>
void World::GetComponents(Entity e, Comps&... comps) const {
    (comps = GetComponent<Comps>(e), ...);
}

template <typename C>
void World::SetComponent(Entity e, const C& component) {
    if (GetComponentBuffer<C>().SetComponent(e, component))
        return;
    for (auto& s : systems) {
        const Family& family = s.GetFamily();
        if (family.Contains(GetTypeId<C>()) && Matches(e, family))
            s.Notify(EntityEvent(e, EntityEvent::ADD));
    }
}

template <typename... Comps>
void World::SetComponents(Entity e, const Comps&... comps) const {
    SetComponent(e, comps)...;
}

template <typename C>
bool World::HasComponent(Entity e) const {
    return HasComponent(e, GetTypeId<C>());
}
}  // namespace verna

#endif