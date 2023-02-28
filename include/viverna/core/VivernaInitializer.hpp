#ifndef VERNA_VIVERNA_INITIALIZER_HPP
#define VERNA_VIVERNA_INITIALIZER_HPP

#include "VivernaState.hpp"

namespace verna {
/**
 * @brief Initializes all subsystems (Window, Input, Renderer, ...)
 *
 * @param state Runtime information
 */
void InitializeViverna(VivernaState& state);

/**
 * @brief Terminates all subsystems (Window, Input, Renderer, ...)
 *
 * @param state Runtime information
 */
void TerminateViverna(VivernaState& state);
}  // namespace verna

#endif