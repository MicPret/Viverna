#include <viverna/core/Debug.hpp>
#include <viverna/graphics/NativeWindow.hpp>
#include "../GraphicsAPIHelper.hpp"

namespace verna {

static ANativeWindow* window = nullptr;

void SetNativeWindowPointer(void* ptr) {
    window = static_cast<ANativeWindow*>(ptr);
}

void* GetNativeWindowPointer() {
    return window;
}

void SwapNativeWindowBuffers() {
    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    VERNA_ASSERT(window != nullptr);
    VERNA_ASSERT(display != EGL_NO_DISPLAY);
    VERNA_ASSERT(surface != EGL_NO_SURFACE);
    eglSwapBuffers(display, surface);
    VERNA_ASSERT(eglGetError() == EGL_SUCCESS);
}

}  // namespace verna