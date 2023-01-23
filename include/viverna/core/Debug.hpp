#ifndef VERNA_DEBUG_HPP
#define VERNA_DEBUG_HPP

#include <string_view>

namespace verna::detail {
void LogInfo(std::string_view message);
void LogWarning(std::string_view message);
void LogError(std::string_view message);
}  // namespace verna::detail

#ifndef NDEBUG
#define VERNA_LOGI(x) verna::detail::LogInfo(x)
#define VERNA_LOGW(x) verna::detail::LogWarning(x)
#define VERNA_LOGE(x) verna::detail::LogError(x)
#define VERNA_LOGI_IF(cond, x) \
    if (cond)                  \
    VERNA_LOGI(x)
#define VERNA_LOGW_IF(cond, x) \
    if (cond)                  \
    VERNA_LOGW(x)
#define VERNA_LOGE_IF(cond, x) \
    if (cond)                  \
    VERNA_LOGE(x)
#include <cassert>
#include <string>
#define VERNA_ASSERT(x)                                      \
    if (!(x)) {                                              \
        VERNA_LOGE("ASSERT FAILED! File " __FILE__ ", line " \
                   + std::to_string(__LINE__));              \
        assert(false);                                       \
    }
#else
#define VERNA_LOGI(x)
#define VERNA_LOGW(x)
#define VERNA_LOGE(x)
#define VERNA_LOGI_IF(cond, x)
#define VERNA_LOGW_IF(cond, x)
#define VERNA_LOGE_IF(cond, x)
#define VERNA_ASSERT(x)
#endif

#endif