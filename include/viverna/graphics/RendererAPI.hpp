#ifndef VERNA_RENDERER_API_HPP
#define VERNA_RENDERER_API_HPP

#include <viverna/core/VivernaState.hpp>

namespace verna {
/**
 * @brief Initializes the renderer API. Must be called after InitializeWindow()
 *
 */
void InitializeRendererAPI(VivernaState& state);

/**
 * @brief Terminates the renderer API, releasing its resources. Must be called
 * after InitializeRenderer()
 *
 */
void TerminateRendererAPI(VivernaState& state);
}  // namespace verna

#endif