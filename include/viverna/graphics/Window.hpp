#ifndef VERNA_WINDOW_HPP
#define VERNA_WINDOW_HPP

#include <viverna/core/VivernaState.hpp>

namespace verna {
/**
 * @brief Initializes the window
 *
 * @param state
 */
void InitializeWindow(VivernaState& state);
/**
 * @brief Terminates the window, releasing its resources. Must be called after
 * TerminateRendererAPI()
 *
 * @param state
 */
void TerminateWindow(VivernaState& state);
int WindowWidth();
int WindowHeight();
}  // namespace verna

#endif