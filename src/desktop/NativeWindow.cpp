#include <viverna/core/Debug.hpp>
#include <viverna/graphics/GraphicsAPIHelper.hpp>
#include <viverna/graphics/NativeWindow.hpp>

namespace verna {

static GLFWwindow* window = nullptr;

void SetNativeWindowPointer(void* ptr) {
    VERNA_ASSERT(glfwGetCurrentContext() == static_cast<GLFWwindow*>(ptr));
    window = static_cast<GLFWwindow*>(ptr);
}

void* GetNativeWindowPointer() {
    return window;
}

void SwapBuffers() {
    VERNA_ASSERT(window != nullptr);
    VERNA_ASSERT(window == glfwGetCurrentContext());
    glfwSwapBuffers(window);
    glfwPollEvents();
}

}  // namespace verna