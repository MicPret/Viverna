#ifndef VERNA_EDITOR_ENTITY_NAME
#define VERNA_EDITOR_ENTITY_NAME

#include <string>

namespace editor {
class EntityName {
   public:
    EntityName() = default;
    EntityName(const char* c_str) : str(c_str) {}
    std::string* operator->() { return &str; };
    auto& String() { return str; }
    const auto& String() const { return str; }

   private:
    std::string str;
};
}  // namespace editor

#endif
