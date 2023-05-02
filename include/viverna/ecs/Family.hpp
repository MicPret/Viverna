#ifndef VERNA_FAMILY_HPP
#define VERNA_FAMILY_HPP

#include <viverna/core/TypeId.hpp>
#include <vector>

namespace verna {
class Family {
   public:
    explicit Family(std::initializer_list<TypeId> types_);
    size_t Size() const;
    bool Contains(TypeId type) const;
    bool operator==(const Family& other) const;
    auto begin() { return types.begin(); }
    auto end() { return types.end(); }
    auto begin() const { return types.begin(); }
    auto end() const { return types.end(); }
    // Same as Family({GetTypeId<T0>(), GetTypeId<T1>(), ...})
    template <typename... T>
    static Family From();

   private:
    std::vector<TypeId> types;
};

template <typename... T>
Family Family::From() {
    return Family({GetTypeId<T>()...});
}

}  // namespace verna

#endif
