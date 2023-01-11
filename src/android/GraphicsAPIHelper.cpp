#include <viverna/core/Debug.hpp>
#include <viverna/graphics/NativeWindow.hpp>
#include "../GraphicsAPIHelper.hpp"

#include <string>
#include <vector>

namespace verna {

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;

static bool InitEGL();
static bool CheckEGLError(std::string& codename);

bool InitGraphicsAPI() {
    if (GetNativeWindowPointer() == nullptr) {
        VERNA_LOGE("Use SetNativeWindowPointer(void* ptr)!");
        return false;
    }
    if (!InitEGL())
        return false;
    // Check openGL on the system
    std::string info =
        reinterpret_cast<const char*>(glGetString(GL_VENDOR))
        + std::to_string(' ')
        + reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    VERNA_LOGI("GPU: " + info);
    info = std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    VERNA_LOGI("API: " + info);

    return true;
}

void TermGraphicsAPI() {
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
}

static bool InitEGL() {
    std::string error;
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!CheckEGLError(error)) {
        VERNA_LOGE("eglGetDisplay failed: " + error);
        return false;
    }

    EGLint egl_major, egl_minor;
    if (eglInitialize(display, &egl_major, &egl_minor) == EGL_FALSE) {
        CheckEGLError(error);
        VERNA_LOGE("eglInitialize failed: " + error);
        return false;
    }
    VERNA_LOGI("eglInitialize succeeded: version " + std::to_string(egl_major)
               + '.' + std::to_string(egl_minor));

    constexpr EGLint BITS_PER_CHANNEL = 8;
    constexpr EGLint BITS_PER_DEPTH = 24;
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,   EGL_BLUE_SIZE,  BITS_PER_CHANNEL,
        EGL_GREEN_SIZE,   BITS_PER_CHANNEL, EGL_RED_SIZE,   BITS_PER_CHANNEL,
        EGL_ALPHA_SIZE,   BITS_PER_CHANNEL, EGL_DEPTH_SIZE, BITS_PER_DEPTH,
        EGL_NONE};
    EGLint num_configs;
    if (eglChooseConfig(display, attribs, nullptr, 0, &num_configs)
        == EGL_FALSE) {
        CheckEGLError(error);
        VERNA_LOGE("eglChooseConfig failed: " + error);
        return false;
    }
    std::vector<EGLConfig> supported_configs(num_configs);
    if (eglChooseConfig(display, attribs, supported_configs.data(), num_configs,
                        &num_configs)
        == EGL_FALSE) {
        CheckEGLError(error);
        VERNA_LOGE("eglChooseConfig failed: " + error);
        return false;
    }
    EGLint i;
    EGLConfig config;
    for (i = 0; i < num_configs; i++) {
        const auto& cfg = supported_configs[i];
        EGLint r, g, b, a, d;
        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r)
            && eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g)
            && eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b)
            && eglGetConfigAttrib(display, cfg, EGL_ALPHA_SIZE, &a)
            && eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d)
            && r == BITS_PER_CHANNEL && g == BITS_PER_CHANNEL
            && b == BITS_PER_CHANNEL && a == BITS_PER_CHANNEL
            && d == BITS_PER_DEPTH) {
            config = cfg;
            break;
        }
    }
    if (i == num_configs) {
        config = supported_configs[0];
    }

    if (config == nullptr) {
        VERNA_LOGE("Unable to initialize EGLConfig");
        return false;
    }

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    EGLNativeWindowType window =
        static_cast<EGLNativeWindowType>(GetNativeWindowPointer());
    surface = eglCreateWindowSurface(display, config, window, nullptr);
    if (!CheckEGLError(error)) {
        VERNA_LOGE("eglCreateWindowSurface failed: " + error);
        return false;
    }
    const EGLint attributes[] = {EGL_CONTEXT_MAJOR_VERSION,
                                 3,
                                 EGL_CONTEXT_MINOR_VERSION,
                                 2,
#ifndef NDEBUG
                                 EGL_CONTEXT_OPENGL_DEBUG,
                                 EGL_TRUE,
#endif
                                 EGL_NONE};
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attributes);
    if (!CheckEGLError(error)) {
        VERNA_LOGE("eglCreateContext failed: " + error);
        return false;
    }

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        CheckEGLError(error);
        VERNA_LOGE("eglMakeCurrent failed: " + error);
        return false;
    }

    EGLint w, h;
    if (eglQuerySurface(display, surface, EGL_WIDTH, &w) == EGL_FALSE) {
        CheckEGLError(error);
        VERNA_LOGE("eglQuerySurface failed: " + error);
        return false;
    }
    if (eglQuerySurface(display, surface, EGL_HEIGHT, &h) == EGL_FALSE) {
        CheckEGLError(error);
        VERNA_LOGE("eglQuerySurface failed: " + error);
        return false;
    }
    VERNA_LOGI("Display width:  " + std::to_string(w));
    VERNA_LOGI("Display height: " + std::to_string(h));
    return true;
}

static bool CheckEGLError(std::string& codename) {
    const EGLint error = eglGetError();
    switch (error) {
        case EGL_SUCCESS:
            return true;
        case EGL_NOT_INITIALIZED:
            codename = "EGL_NOT_INITIALIZED";
            break;
        case EGL_BAD_ACCESS:
            codename = "EGL_BAD_ACCESS";
            break;
        case EGL_BAD_ALLOC:
            codename = "BAD_ALLOC";
            break;
        case EGL_BAD_ATTRIBUTE:
            codename = "BAD_ATTRIBUTE";
            break;
        case EGL_BAD_CONTEXT:
            codename = "EGL_BAD_CONTEXT";
            break;
        case EGL_BAD_CONFIG:
            codename = "EGL_BAD_CONFIG";
            break;
        case EGL_BAD_CURRENT_SURFACE:
            codename = "EGL_BAD_CURRENT_SURFACE";
            break;
        case EGL_BAD_DISPLAY:
            codename = "EGL_BAD_DISPLAY";
            break;
        case EGL_BAD_SURFACE:
            codename = "EGL_BAD_SURFACE";
            break;
        case EGL_BAD_MATCH:
            codename = "EGL_BAD_MATCH";
            break;
        case EGL_BAD_PARAMETER:
            codename = "EGL_BAD_PARAMETER";
            break;
        case EGL_BAD_NATIVE_PIXMAP:
            codename = "EGL_BAD_NATIVE_PIXMAP";
            break;
        case EGL_BAD_NATIVE_WINDOW:
            codename = "EGL_BAD_NATIVE_WINDOW";
            break;
        case EGL_CONTEXT_LOST:
            codename = "EGL_CONTEXT_LOST";
            break;
    }
    return false;
}

namespace gpu {
int MaxTextureUnits() {
    static GLint max_frag_tus = [] {
        GLint x;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &x);
        return x;
    }();
    return max_frag_tus;
}
}  // namespace gpu

}  // namespace verna
