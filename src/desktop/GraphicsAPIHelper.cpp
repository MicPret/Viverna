#include <viverna/core/Debug.hpp>
#include <viverna/graphics/GraphicsAPIHelper.hpp>
#include <viverna/graphics/NativeWindow.hpp>

#include <string>

namespace verna {

static bool initialized = false;
static void OpenGLDebugOutputFunc(GLenum source,
                                  GLenum type,
                                  GLuint id,
                                  GLenum severity,
                                  GLsizei length,
                                  const GLchar* message,
                                  const void* userParam);

bool InitGraphicsAPI() {
    if (initialized)
        return true;
#ifndef NDEBUG
    // TODO error -> error_name
    glfwSetErrorCallback([](int error, const char* description) {
        VERNA_LOGE("GLFW error " + std::to_string(error) + ":\n" + description);
    });
#endif

    if (glfwInit() == GLFW_FALSE) {
        VERNA_LOGE("glfwInit() failed!");
        return false;
    }
    initialized = true;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor == nullptr) {
        VERNA_LOGE("glfwGetPrimaryMonitor() failed!");
        TermGraphicsAPI();
        return false;
    }
    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
    if (monitor == nullptr) {
        VERNA_LOGE("glfwGetVideoMode(monitor) failed!");
        TermGraphicsAPI();
        return false;
    }

    glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(vidmode->width, vidmode->height,
                                          "Viverna", monitor, nullptr);
    if (window == nullptr) {
        VERNA_LOGE("glfwCreateWindow() failed!");
        TermGraphicsAPI();
        return false;
    }
    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        VERNA_LOGE("gladLoadGL(glfwGetProcAddress) failed!");
        TermGraphicsAPI();
        return false;
    }
    VERNA_LOGI("Loaded OpenGL " + std::to_string(GLAD_VERSION_MAJOR(version))
               + '.' + std::to_string(GLAD_VERSION_MINOR(version)));

#ifndef NDEBUG
    glDebugMessageCallback(OpenGLDebugOutputFunc, window);
#endif
    SetNativeWindowPointer(window);
    return true;
}

void TermGraphicsAPI() {
    if (!initialized)
        return;
    GLFWwindow* window = glfwGetCurrentContext();
    if (window != nullptr)
        glfwDestroyWindow(window);
    SetNativeWindowPointer(nullptr);
    glfwTerminate();
    initialized = false;
}

static void OpenGLDebugOutputFunc(GLenum source,
                                  GLenum type,
                                  GLuint id,
                                  GLenum severity,
                                  GLsizei length,
                                  const GLchar* message,
                                  const void* userParam) {
    // TODO better OpenGL error handling
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            VERNA_LOGE("OpenGL error " + std::to_string(id) + ": " + message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            VERNA_LOGW("OpenGL warning " + std::to_string(id) + ": " + message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            VERNA_LOGI("OpenGL info " + std::to_string(id) + ": " + message);
            break;
        default:
            break;
    }
}
}  // namespace verna