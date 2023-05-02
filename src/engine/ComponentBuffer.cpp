#include <viverna/ecs/ComponentBuffer.hpp>

namespace verna {
void BaseComponentBuffer::Clear() {
    sparse_set.Clear();
}
bool BaseComponentBuffer::Contains(Entity e) const {
    return sparse_set.Contains(e.id);
}
}  // namespace verna
