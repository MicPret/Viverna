#include <viverna/core/Debug.hpp>

#include <android/log.h>

namespace verna::detail {

static constexpr auto TAG = "Viverna";

void LogInfo(std::string_view message) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "%s", message.data());
}

void LogWarning(std::string_view message) {
    __android_log_print(ANDROID_LOG_WARN, TAG, "%s", message.data());
}

void LogError(std::string_view message) {
    __android_log_print(ANDROID_LOG_ERROR, TAG, "%s", message.data());
}

}  // namespace verna::detail