#ifndef VERNA_RENDERER_HPP
#define VERNA_RENDERER_HPP

#include <viverna/maths/Mat4f.hpp>
#include "Mesh.hpp"
#include "Shader.hpp"

namespace verna {
/**
 * @brief Initializes the renderer. Must be followed by TerminateRenderer()
 *
 */
void InitializeRenderer();

/**
 * @brief Terminates the renderer, releasing its resources. Must be called after
 * InitializeRenderer()
 *
 */
void TerminateRenderer();

/**
 * @brief Renders an element on the back buffer, which is not shown on screen
 * until Draw() is called
 *
 * @param mesh The mesh that must be rendered
 * @param material The material properties used to render the mesh
 * @param transform_matrix The matrix used to give the mesh a position, rotation
 * and scale
 * @param shader_id The identifier for the shader program to use to render the
 * mesh
 */
void Render(const Mesh& mesh,
            const Material& material,
            const Mat4f& transform_matrix,
            ShaderId shader_id);

/**
 * @brief Updates the screen with the content rendered on the back buffer
 *
 */
void Draw();
}  // namespace verna

#endif