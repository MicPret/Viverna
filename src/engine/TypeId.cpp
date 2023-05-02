#include <viverna/core/TypeId.hpp>

namespace verna {
namespace detail {
TypeId NewTypeId() {
    static TypeId id = 0;
    return ++id;
}
}  // namespace detail
}  // namespace verna