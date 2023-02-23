#include <viverna/graphics/RendererAPI.hpp>
#include <viverna/core/Debug.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <string_view>
#include <string>

namespace verna {

[[maybe_unused]] static void OpenGLDebugOutputFunc(GLenum source,
                                                   GLenum type,
                                                   GLuint id,
                                                   GLenum severity,
                                                   GLsizei length,
                                                   const GLchar* message,
                                                   const void* userParam);

static void RendererAPIError(VivernaState& state,
                             [[maybe_unused]] std::string_view message) {
    VERNA_LOGE(message);
    state.SetFlag(VivernaState::ERROR_FLAG, true);
}

void InitializeRendererAPI(VivernaState& state) {
    if (state.GetFlag(VivernaState::RENDERER_API_INITIALIZED_FLAG)) {
        return;
    }
    if (state.native_window == nullptr) {
        RendererAPIError(state,
                         "InitializeWindow() should be called before "
                         "InitializeRendererAPI()!");
        return;
    }

    [[maybe_unused]] int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        RendererAPIError(state, "gladLoadGL(glfwGetProcAddress) failed!");
        return;
    }
#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLDebugOutputFunc, &state);
#endif

    state.SetFlag(VivernaState::RENDERER_API_INITIALIZED_FLAG, true);
    VERNA_LOGI("Renderer API initialized!\t\t\tOpenGL "
               + std::to_string(GLAD_VERSION_MAJOR(version)) + '.'
               + std::to_string(GLAD_VERSION_MINOR(version)));
}

[[maybe_unused]] static void OpenGLDebugOutputFunc(GLenum source,
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

void TerminateRendererAPI(VivernaState& state) {
    if (!state.GetFlag(VivernaState::RENDERER_API_INITIALIZED_FLAG))
        return;
    if (state.GetFlag(VivernaState::RENDERER_INITIALIZED_FLAG)) {
        VERNA_LOGE(
            "TerminateRenderer() should be called before "
            "TerminateRendererAPI()");
        state.SetFlag(VivernaState::ERROR_FLAG, true);
        return;
    }
    state.SetFlag(VivernaState::RENDERER_API_INITIALIZED_FLAG, false);
    VERNA_LOGI("Renderer API terminated!");
}
}  // namespace verna