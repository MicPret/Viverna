#include <viverna/core/Input.hpp>
#include <viverna/core/Debug.hpp>

#include <GLFW/glfw3.h>

#include <cstdint>

namespace verna {

static uint32_t mouse;

static void SetClick(bool click) {
    constexpr uint32_t mask = ~(1 << 31);
    mouse = (mouse & mask) | (static_cast<uint32_t>(click) << 31);
}
static void SetMousePos(uint32_t x, uint32_t y) {
    uint32_t m = mouse & (1 << 31);
    mouse = m | (x << 15) | y;
}
static bool GetClick() {
    return (mouse >> 31) != 0;
}
static void GetMousePos(unsigned& x, unsigned& y) {
    constexpr uint32_t mask = ~(1 << 31);
    uint32_t m = mouse & mask;
    x = m >> 15;
    y = m & 0x7FFF;
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

}  // namespace verna