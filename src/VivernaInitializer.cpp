#include <viverna/core/VivernaInitializer.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/core/Input.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/RendererAPI.hpp>
#include <viverna/graphics/Window.hpp>

namespace verna {
static bool GetError(const VivernaState& state) {
    return state.GetFlag(VivernaState::ERROR_FLAG);
}

void InitializeAll(VivernaState& state) {
    if (GetError(state)) {
        VERNA_LOGE("Could not initialize the engine");
        return;
    }

    InitializeWindow(state);
    if (GetError(state))
        return;

    InitializeRendererAPI(state);
    if (GetError(state)) {
        TerminateWindow(state);
        return;
    }

    InitializeRenderer(state);
    if (GetError(state)) {
        TerminateRendererAPI(state);
        TerminateWindow(state);
        return;
    }

    InitializeInput(state);
    if (GetError(state)) {
        TerminateRenderer(state);
        TerminateRendererAPI(state);
        TerminateWindow(state);
        return;
    }

    state.SetFlag(VivernaState::RUNNING_FLAG, true);
    state.epoch = Clock::now();
}

void TerminateAll(VivernaState& state) {
    if (GetError(state)) {
        VERNA_LOGE("An error was raised before terminating the engine");
        state.SetFlag(VivernaState::ERROR_FLAG, false);
    }

    TerminateInput(state);

    TerminateRenderer(state);
    if (GetError(state))
        return;

    TerminateRendererAPI(state);
    if (GetError(state))
        return;

    TerminateWindow(state);
    if (GetError(state))
        return;
    state.SetFlag(VivernaState::RUNNING_FLAG, false);
}
}  // namespace verna