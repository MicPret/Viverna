#include <viverna/core/Debug.hpp>

#include <iostream>

namespace verna::detail {

static void Log(std::string_view priority, std::string_view msg) {
    std::cout << '[' << priority << "] " << msg << std::endl;
}

void LogInfo(std::string_view message) {
    Log("INFO", message);
}

void LogWarning(std::string_view message) {
    Log("WARN", message);
}

void LogError(std::string_view message) {
    Log("ERROR", message);
}

}  // namespace verna::detail