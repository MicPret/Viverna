#ifndef VERNA_ENTITY_NAME_HPP
#define VERNA_ENTITY_NAME_HPP

#include <string>
#include <string_view>

namespace verna {
struct EntityName {
    std::string str;
    EntityName() = default;
    EntityName(std::string_view name);
    EntityName(std::string&& name);
    const std::string* operator->() const;
    std::string* operator->();
};
}  // namespace verna

#endif
