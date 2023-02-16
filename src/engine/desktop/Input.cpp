#include <viverna/core/Input.hpp>
#include <viverna/core/Debug.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <array>
#include <cstdint>

namespace verna {

static uint32_t mouse;
static std::array<uint8_t, static_cast<size_t>(Key::Last) / 8> keymap;

static bool GetClick() {
    return (mouse >> 31) != 0;
}
static void SetClick(bool click) {
    constexpr uint32_t mask = ~(1 << 31);
    mouse = (mouse & mask) | (static_cast<uint32_t>(click) << 31);
}

static void GetMousePos(unsigned& x, unsigned& y) {
    constexpr uint32_t mask = ~(1 << 31);
    uint32_t m = mouse & mask;
    x = m >> 15;
    y = m & 0x7FFF;
}
static void SetMousePos(uint32_t x, uint32_t y) {
    uint32_t m = mouse & (1 << 31);
    mouse = m | (x << 15) | y;
}

static constexpr int GetKeyIndex(Key key, int& offset) {
    int k = static_cast<int>(key);
    offset = k % 8;
    return k / 8;
}
static constexpr bool GetKeyPressed(Key key) {
    int offset = 0;
    int index = GetKeyIndex(key, offset);
    int keys = static_cast<int>(keymap[index]);
    int mask = 1 << offset;
    return (keys & mask) != 0;
}
static constexpr void SetKeyPressed(Key key, bool pressed) {
    int offset = 0;
    int index = GetKeyIndex(key, offset);
    int keys = static_cast<int>(keymap[index]);
    int mask = 1 << offset;
    keys = (keys & ~mask) | (static_cast<int>(pressed) << offset);
    keymap[index] = static_cast<uint8_t>(keys);
}

static void MouseButtonCallback(GLFWwindow* window,
                                int button,
                                int action,
                                int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        SetClick(action == GLFW_PRESS);
}

static void CursorCallback(GLFWwindow* window, double x, double y) {
    SetMousePos(static_cast<uint32_t>(x), static_cast<uint32_t>(y));
}

static void KeyCallback(GLFWwindow* window,
                        int key,
                        int scancode,
                        int action,
                        int mods) {
    if (key >= static_cast<int>(Key::Last) || key < 0)
        return;
    SetKeyPressed(static_cast<Key>(key), action == GLFW_PRESS);
}

void InitializeInput(VivernaState& state) {
    if (state.GetFlag(VivernaState::INPUT_INITIALIZED_FLAG))
        return;
    if (state.native_window == nullptr) {
        VERNA_LOGE("InitializeInput must be called after InitializeWindow!");
        state.SetFlag(VivernaState::ERROR_FLAG, true);
        return;
    }
    GLFWwindow* window = static_cast<GLFWwindow*>(state.native_window);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorCallback);
    glfwSetKeyCallback(window, KeyCallback);
    mouse = 0;
    keymap.fill(0u);
    state.SetFlag(VivernaState::INPUT_INITIALIZED_FLAG, true);
    VERNA_LOGI("Input initialized!");
}

void TerminateInput(VivernaState& state) {
    if (!state.GetFlag(VivernaState::INPUT_INITIALIZED_FLAG))
        return;
    if (state.native_window == nullptr) {
        VERNA_LOGE("TerminateInput must be called before TerminateWindow!");
        return;
    }
    GLFWwindow* window = static_cast<GLFWwindow*>(state.native_window);
    glfwSetMouseButtonCallback(window, nullptr);
    glfwSetCursorPosCallback(window, nullptr);
    state.SetFlag(VivernaState::INPUT_INITIALIZED_FLAG, false);
    VERNA_LOGI("Input terminated!");
}

void MouseListener::Position(unsigned& pos_x, unsigned& pos_y) const {
    GetMousePos(pos_x, pos_y);
}

bool MouseListener::Pressed(unsigned& pos_x, unsigned& pos_y) const {
    Position(pos_x, pos_y);
    return GetClick();
}

bool KeyListener::Pressed() const {
    return GetKeyPressed(key);
}

}  // namespace verna