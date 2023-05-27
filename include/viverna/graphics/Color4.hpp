#ifndef VERNA_COLOR4_HPP
#define VERNA_COLOR4_HPP

#include <cstdint>
#include <cstddef>
#include <type_traits>

template <typename T>
struct Color4 {
    using channel_t = T;
    channel_t red;
    channel_t green;
    channel_t blue;
    channel_t alpha;
    constexpr Color4() : red(0), green(0), blue(0), alpha(0) {}
    constexpr Color4(channel_t r, channel_t g, channel_t b, channel_t a) :
        red(r), green(g), blue(b), alpha(a) {}
    inline channel_t* Data() { return &red; }
    inline const channel_t* Data() const { return &red; }
    static constexpr size_t ChannelByteSize() { return sizeof(channel_t); }
    static_assert(std::is_arithmetic_v<channel_t>);
};

using Color4u8 = Color4<uint8_t>;
using Color4f = Color4<float>;

constexpr bool operator==(Color4u8 a, Color4u8 b) {
    return (a.red == b.red) && (a.green == b.green) && (a.blue == b.blue)
           && (a.alpha == b.alpha);
}

#endif
