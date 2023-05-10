#include <viverna/ecs/EntityName.hpp>

#include <utility>

namespace verna {

EntityName::EntityName(std::string_view name) : str(name) {}
EntityName::EntityName(std::string&& name) : str(std::move(name)) {}

const std::string* EntityName::operator->() const {
    return &str;
}
std::string* EntityName::operator->() {
    return &str;
}

}  // namespace verna
