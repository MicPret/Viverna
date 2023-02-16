#include <game/core/Application.hpp>
#include <viverna/core/VivernaInitializer.hpp>

static void MainLoop(verna::VivernaState& verna_state);

int main() {
    verna::VivernaInitializerRAII initializer;
    auto& app_state = initializer.app_state;
    verna::OnAppResume(app_state);

    while (app_state.GetFlag(verna::VivernaState::RUNNING_FLAG))
        MainLoop(app_state);

    verna::OnAppPause(app_state);

    return 0;
}

static void MainLoop(verna::VivernaState& verna_state) {
    static auto last = verna::Clock::now();
    auto now = verna::Clock::now();
    verna::DeltaTime<float, verna::Seconds> dt = now - last;
    verna::OnAppUpdate(verna_state, dt);
    last = now;
}