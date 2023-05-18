#include <viverna/ecs/ComponentBuffer.hpp>

namespace verna {
void BaseComponentBuffer::Clear() {
    sparse_set.Clear();
}
bool BaseComponentBuffer::Contains(Entity e) const {
    return sparse_set.Contains(e.id);
}
std::vector<Entity> BaseComponentBuffer::GetEntities() const {
    std::vector<Entity> result;
    const auto& ids = sparse_set.GetDense();
    size_t n = ids.size();
    result.reserve(n);
    Entity e;
    for (size_t i = 0; i < n; i++) {
        e.id = ids[i];
        if (Contains(e))
            result.push_back(e);
    }
    return result;
}
}  // namespace verna
