#ifndef VERNA_COMPONENT_BUFFER_HPP
#define VERNA_COMPONENT_BUFFER_HPP

#include "Entity.hpp"
#include <viverna/core/Debug.hpp>
#include <viverna/data/SparseSet.hpp>

namespace verna {

class BaseComponentBuffer {
   public:
    BaseComponentBuffer() = default;
    virtual ~BaseComponentBuffer() = default;
    const auto& GetEntityIds() const { return sparse_set.GetDense(); }
    virtual TypeId GetType() const = 0;
    bool Contains(Entity e) const;
    void Clear();

   protected:
    SparseSet<EntityId> sparse_set;
};

template <typename C>
class ComponentBuffer : public BaseComponentBuffer {
   public:
    C GetComponent(Entity e) const;
    // Returns false if it's a new component
    bool SetComponent(Entity e, const C& component);
    const auto& GetComponents() const { return components; }
    auto& GetComponents() { return components; }
    TypeId GetType() const override { return GetTypeId<C>(); }

   private:
    std::vector<C> components;
};

template <typename C>
C ComponentBuffer<C>::GetComponent(Entity e) const {
    SparseSet<EntityId>::index_t i;
    [[maybe_unused]] bool found = sparse_set.GetIndex(e.id, i);
    VERNA_LOGE_IF(!found,
                  "Component not found for Entity " + std::to_string(e.id));
    return components[i];
}

template <typename C>
bool ComponentBuffer<C>::SetComponent(Entity e, const C& component) {
    SparseSet<EntityId>::index_t i;
    if (sparse_set.GetIndex(e.id, i)) {
        components[i] = component;
        return true;
    }
    sparse_set.Add(e.id);
    components.push_back(component);
    return false;
}
}  // namespace verna

#endif
