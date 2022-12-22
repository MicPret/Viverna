#include <viverna/core/Debug.hpp>

#include <iostream>

namespace verna::detail {

void LogInfo(std::string_view message) {
    std::cout << "[INFO] " << message << '\n';
}

void LogWarning(std::string_view message) {
    std::cout << "[WARN] " << message << '\n';
}

void LogError(std::string_view message) {
    std::cout << "[ERROR] " << message << '\n';
}

}  // namespace verna::detail