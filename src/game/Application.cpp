#include <game/core/Application.hpp>

#include <viverna/core/Input.hpp>
#include <viverna/graphics/Renderer.hpp>

namespace verna {
void OnAppResume(VivernaState& app_state) {
    // Called when the application (re)starts
}
void OnAppPause(VivernaState& app_state) {
    // Called when the application is paused (or exited)
}
void OnAppUpdate(VivernaState& app_state, DeltaTime<float, Seconds> dt) {
    // Called every frame (remember to call Draw() and NextFrame()!)

    NextFrame();

    KeyListener escape(Key::Escape);
    if (escape.Pressed()) {
        app_state.SetFlag(VivernaState::RUNNING_FLAG, false);
        return;
    }

    Draw();
}

}  // namespace verna
