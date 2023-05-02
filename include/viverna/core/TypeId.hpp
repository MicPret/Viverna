#ifndef VERNA_TYPE_ID_HPP
#define VERNA_TYPE_ID_HPP

#include <cstdint>

namespace verna {

using TypeId = uint32_t;

namespace detail {
TypeId NewTypeId();
}  // namespace detail

template <typename T>
TypeId GetTypeId() {
    static TypeId id = detail::NewTypeId();
    return id;
}

}  // namespace verna

#endif
