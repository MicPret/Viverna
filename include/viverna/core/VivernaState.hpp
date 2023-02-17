#ifndef VERNA_VIVERNA_STATE_HPP
#define VERNA_VIVERNA_STATE_HPP

#include "Time.hpp"

#include <cstdint>

namespace verna {
struct VivernaState {
    using flag_type = uint8_t;

    void* native_app;
    void* native_window;
    TimePoint epoch;
    flag_type flags;

    static constexpr flag_type ERROR_FLAG = 1 << 0;
    static constexpr flag_type RENDERER_API_INITIALIZED_FLAG = 1 << 1;
    static constexpr flag_type RENDERER_INITIALIZED_FLAG = 1 << 2;
    static constexpr flag_type INPUT_INITIALIZED_FLAG = 1 << 3;
    static constexpr flag_type ASSETS_INITIALIZED_FLAG = 1 << 4;
    static constexpr flag_type RUNNING_FLAG = 1 << 5;

    constexpr VivernaState() :
        native_app(nullptr), native_window(nullptr), flags(0) {}
    constexpr bool GetFlag(flag_type flag) const { return (flags & flag) != 0; }
    constexpr void SetFlag(flag_type flag, bool set) {
        flags = (flags & ~flag) | (flag * set);
    }
};
}  // namespace verna

#endif