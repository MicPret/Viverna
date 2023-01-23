#include <viverna/graphics/RendererAPI.hpp>
#include <viverna/core/Debug.hpp>

#include <GLES3/gl32.h>
#include <EGL/egl.h>

#include <string_view>
#include <string>

namespace verna {
void InitializeRendererAPI(VivernaState& state) {
    if (state.GetFlag(VivernaState::RENDERER_API_INITIALIZED_FLAG)) {
        return;
    }
    if (state.native_window == nullptr) {
        state.SetFlag(VivernaState::ERROR_FLAG, true);
        VERNA_LOGE(
            "InitializeRendererAPI() needs a not-null state.native_window");
        return;
    }

    // Check openGL on the system
    [[maybe_unused]] std::string info =
        reinterpret_cast<const char*>(glGetString(GL_VENDOR))
        + std::to_string(' ')
        + reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    VERNA_LOGI("GPU: " + info);
    info.assign(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    VERNA_LOGI("API: " + info);

    state.SetFlag(VivernaState::RENDERER_API_INITIALIZED_FLAG, true);
    VERNA_LOGI("Renderer API initialized!");
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