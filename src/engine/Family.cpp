#include <viverna/ecs/Family.hpp>

#include <set>

namespace verna {
Family::Family(std::initializer_list<TypeId> types_) {
    std::set<TypeId> temp(types_);
    types.assign(temp.begin(), temp.end());
}

size_t Family::Size() const {
    return types.size();
}

bool Family::Contains(TypeId type) const {
    for (size_t i = 0; i < types.size(); i++)
        if (type == types[i])
            return true;
    return false;
}

bool Family::operator==(const Family& other) const {
    if (Size() != other.Size())
        return false;
    for (size_t i = 0; i < types.size(); i++)
        if (types[i] != other.types[i])
            return false;
    return true;
}
}  // namespace verna