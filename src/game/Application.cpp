#include <game/core/Application.hpp>

namespace verna {
void OnAppResume(VivernaState& app_state) {
    // Called when the application (re)starts
}
void OnAppPause(VivernaState& app_state) {
    // Called when the application is paused
}
void OnAppUpdate(VivernaState& app_state, DeltaTime<float, Seconds> dt) {
    // Called every frame (remember to call Draw()!)
}

}  // namespace verna
