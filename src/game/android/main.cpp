#include <jni.h>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <string>

#include <android_native_app_glue.h>

#include <viverna/core/Debug.hpp>
#include <viverna/core/VivernaInitializer.hpp>
#include <game/core/Application.hpp>
// #include <game/core/Assets.hpp>

static void HandleAppCmd(struct android_app* app, int32_t cmd);
static void MainLoop(verna::VivernaState& verna_state);

void android_main(struct android_app* state) {
    verna::VivernaState verna_state;
    state->userData = &verna_state;
    state->onAppCmd = HandleAppCmd;
    verna_state.native_app = state;

    if (state->savedState != nullptr) {
    }

    while (true) {
        int ident;
        struct android_poll_source* source;

        while ((ident = ALooper_pollAll(0, nullptr, nullptr, (void**)&source))
               >= 0) {
            if (source == nullptr)
                continue;
            source->process(state, source);
        }

        if (verna_state.GetFlag(verna::VivernaState::RUNNING_FLAG))
            MainLoop(verna_state);
    }
}

static void MainLoop(verna::VivernaState& verna_state) {
    static auto last = verna::Clock::now();
    auto now = verna::Clock::now();
    verna::DeltaTime<float, verna::Seconds> dt = now - last;
    verna::OnAppUpdate(verna_state, dt);
    last = now;
}

static void HandleAppCmd(struct android_app* app, int32_t cmd) {
    VERNA_LOGE_IF(app == nullptr,
                  "HandleAppCmd failed: app should not be null!");
    void* verna_state_ptr = app->userData;
    VERNA_LOGE_IF(verna_state_ptr == nullptr,
                  "HandleAppCmd failed: app->userData should not be null!");
    verna::VivernaState& verna_state =
        *(static_cast<verna::VivernaState*>(verna_state_ptr));
    switch (cmd) {
        case APP_CMD_START:
            VERNA_LOGI("APP_CMD_START");
            break;
        case APP_CMD_INIT_WINDOW:
            VERNA_LOGI("APP_CMD_INIT_WINDOW");
            VERNA_ASSERT(app->window != nullptr);
            verna_state.native_app = app;
            verna_state.native_window = app->window;
            verna::InitializeViverna(verna_state);
            verna::OnAppResume(verna_state);
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            break;
        case APP_CMD_TERM_WINDOW:
            VERNA_LOGI("APP_CMD_TERM_WINDOW");
            verna::OnAppPause(verna_state);
            verna::TerminateViverna(verna_state);
            break;
        case APP_CMD_RESUME:
            VERNA_LOGI("APP_CMD_RESUME");
            break;
        case APP_CMD_PAUSE:
            VERNA_LOGI("APP_CMD_PAUSE");
        default:
            break;
    }
}
