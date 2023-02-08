#ifndef VERNA_KEY_HPP
#define VERNA_KEY_HPP

namespace verna {
#if defined(VERNA_DESKTOP)
enum class Key : short {
    Unknown = 0,
    Space = 32,
    A = 65,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    Escape = 256,
    Enter,
    Tab,
    // Backspace,
    Insert = 260,
    Delete,
    Right,
    Left,
    Down,
    Up,
    PageUp,
    PageDown,
    Home,
    CapsLock = 280,
    ScrollLock,
    NumLock,
    Pause,
    F1 = 290,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    Last
};
#elif defined(VERNA_ANDROID)
enum class Key : short {
    Unknown = 0,
    Home = 3,
    Up = 19,
    Down,
    Left,
    Right,
    A = 29,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    Tab = 61,
    Space,
    Enter = 66,
    PageUp = 92,
    PageDown,
    Escape = 111,
    Delete,
    CapsLock = 115,
    ScrollLock,
    Insert = 124,
    Pause = 127,
    F1 = 131,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    NumLock = 143,
    Last
};
#else
#error Platform not supported!
#endif
}  // namespace verna

#endif