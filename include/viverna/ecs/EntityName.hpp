#ifndef VERNA_ENTITY_NAME_HPP
#define VERNA_ENTITY_NAME_HPP

#include <string>

namespace verna {
struct EntityName {
    std::string str;
    EntityName() = default;
    EntityName(const std::string& name);
    EntityName(std::string&& name);
    const std::string* operator->() const;
    std::string* operator->();
};
}  // namespace verna

#endif
