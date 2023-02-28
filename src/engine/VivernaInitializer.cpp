#include <viverna/core/VivernaInitializer.hpp>
#include <viverna/core/Assets.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/core/Input.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/RendererAPI.hpp>
#include <viverna/graphics/Window.hpp>

namespace verna {

static std::array initializers = {InitializeWindow, InitializeAssets,
                                  InitializeRendererAPI, InitializeRenderer,
                                  InitializeInput};
static std::array terminators = {TerminateWindow, TerminateAssets,
                                 TerminateRendererAPI, TerminateRenderer,
                                 TerminateInput};

static bool GetError(const VivernaState& state) {
    return state.GetFlag(VivernaState::ERROR_FLAG);
}

static void Terminate(int level, VivernaState& state) {
    state.SetFlag(VivernaState::ERROR_FLAG, false);
    for (int i = level; i >= 0; i--) {
        terminators[i](state);
        if (GetError(state))
            return;
    }
}

void InitializeViverna(VivernaState& state) {
    if (GetError(state)) {
        VERNA_LOGE("Could not initialize the engine");
        return;
    }

    constexpr int num_systems = static_cast<int>(initializers.size());
    for (int i = 0; i < num_systems; i++) {
        initializers[i](state);
        if (GetError(state)) {
            Terminate(i - 1, state);
            return;
        }
    }

    state.SetFlag(VivernaState::RUNNING_FLAG, true);
    state.epoch = Clock::now();
}

void TerminateViverna(VivernaState& state) {
    if (GetError(state)) {
        VERNA_LOGE("An error was raised before terminating the engine");
        state.SetFlag(VivernaState::ERROR_FLAG, false);
    }
    Terminate(terminators.size() - 1, state);

    state.SetFlag(VivernaState::RUNNING_FLAG, false);
}
}  // namespace verna