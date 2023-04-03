#ifndef VERNA_APPLICATION_HPP
#define VERNA_APPLICATION_HPP

#include <viverna/core/VivernaState.hpp>

namespace verna {
void OnAppResume(VivernaState& app_state);
void OnAppPause(VivernaState& app_state);
void OnAppUpdate(VivernaState& app_state, DeltaTime<float, Seconds> dt);
}  // namespace verna

#endif