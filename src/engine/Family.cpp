#include <viverna/core/Family.hpp>

#include <set>

namespace verna {
Family::Family(std::initializer_list<TypeId> types_) {
    std::set<TypeId> temp(types_);
    types.assign(temp.begin(), temp.end());
}
}  // namespace verna