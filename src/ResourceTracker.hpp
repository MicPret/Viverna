#ifndef VERNA_RESOURCE_TRACKER_HPP
#define VERNA_RESOURCE_TRACKER_HPP

#include <string>
#include <type_traits>
#include <vector>

namespace verna {
template <typename ID_TYPE>
class ResourceTracker {
    static_assert(std::is_integral_v<ID_TYPE>);

   public:
    ResourceTracker(const std::string& resource_name_) :
        resource_name(resource_name_) {}
    ~ResourceTracker() {
        for (ID_TYPE id : loaded_ids) {
            VERNA_LOGE(resource_name + " not freed! ID: " + std::to_string(id));
        }
    }
    void Push(ID_TYPE id) { loaded_ids.push_back(id); }
    void Remove(ID_TYPE id) {
        for (size_t i = 0; i < loaded_ids.size(); i++) {
            if (loaded_ids[i] == id) {
                loaded_ids.erase(loaded_ids.begin() + i);
                break;
            }
        }
    }

   private:
    std::vector<ID_TYPE> loaded_ids;
    std::string resource_name;
};
}  // namespace verna

#endif