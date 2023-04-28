#include <viverna/core/Entity.hpp>
#include <viverna/core/Family.hpp>

namespace verna {
bool Entity::Matches(const Family& family) const {
    for (TypeId type : family)
        if (!HasComponent(type))
            return false;
    return true;
}
}  // namespace verna
