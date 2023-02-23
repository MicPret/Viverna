#include <viverna/graphics/Window.hpp>
#include <viverna/core/Debug.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <string_view>
#include <string>

namespace verna {

static int width = 0;
static int height = 0;

static void GlfwFailure(VivernaState& state,
                        [[maybe_unused]] std::string_view message) {
    state.SetFlag(VivernaState::ERROR_FLAG, true);
    VERNA_LOGE(message);
    glfwTerminate();
}

static void GlfwWindowHints(const GLFWvidmode* vidmode) {
    glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_DEPTH_BITS, 24); // default value
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
#endif
}

void InitializeWindow(VivernaState& state) {
    if (state.native_window != nullptr) {
        VERNA_LOGI("InitializeWindow called on initialized window! (NOP)");
        return;
    }

#ifndef NDEBUG
    // TODO error -> error_name
    glfwSetErrorCallback([](int error, const char* description) {
        VERNA_LOGE("GLFW error " + std::to_string(error) + ":\n" + description);
    });
#endif

    if (glfwInit() == GLFW_FALSE) {
        GlfwFailure(state, "glfwInit() failed!");
        return;
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor == nullptr) {
        GlfwFailure(state, "glfwGetPrimaryMonitor() failed!");
        return;
    }

    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
    if (vidmode == nullptr) {
        GlfwFailure(state, "glfwGetVideoMode() failed!");
        return;
    }

    GlfwWindowHints(vidmode);

    GLFWwindow* window = glfwCreateWindow(vidmode->width, vidmode->height,
                                          "Viverna", monitor, nullptr);
    if (window == nullptr) {
        GlfwFailure(state, "glfwCreateWindow() failed!");
        return;
    }
    width = vidmode->width;
    height = vidmode->height;
    glfwMakeContextCurrent(window);
    state.native_window = window;

    VERNA_LOGI("Window initialized!");
}

void TerminateWindow(VivernaState& state) {
    if (state.native_window == nullptr) {
        VERNA_LOGW("Tried to terminate Window before initialization!");
        return;
    }
    if (state.GetFlag(VivernaState::RENDERER_API_INITIALIZED_FLAG)) {
        VERNA_LOGE(
            "TerminateRendererAPI() should be called before TerminateWindow()");
        state.SetFlag(VivernaState::ERROR_FLAG, true);
        return;
    }
    GLFWwindow* win = static_cast<GLFWwindow*>(state.native_window);
    glfwDestroyWindow(win);  // Not necessary
    state.native_window = nullptr;
    width = 0;
    height = 0;
    glfwTerminate();
    // TODO glfwSetErrorCallback(nullptr);
    VERNA_LOGI("Window terminated!");
}

int WindowWidth() {
    VERNA_LOGE_IF(width <= 0, "Window was not initialized successfully!");
    return width;
}
int WindowHeight() {
    VERNA_LOGE_IF(height <= 0, "Window was not initialized successfully!");
    return height;
}
}  // namespace verna