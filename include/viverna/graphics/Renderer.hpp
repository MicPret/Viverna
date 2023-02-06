#ifndef VERNA_RENDERER_HPP
#define VERNA_RENDERER_HPP

#include <viverna/core/BoundingBox.hpp>
#include <viverna/core/BoundingSphere.hpp>
#include <viverna/core/VivernaState.hpp>
#include <viverna/maths/Mat4f.hpp>
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Material.hpp"

namespace verna {

/**
 * @brief Initializes the Renderer. Needs InitializeRendererAPI() first
 *
 * @param state The application state
 */
void InitializeRenderer(VivernaState& state);

/**
 * @brief Terminates the Renderer and its resources
 *
 * @param state The application state
 */
void TerminateRenderer(VivernaState& state);

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

void RenderDebug(const BoundingBox& box);
void RenderDebug(const BoundingSphere& sphere);

/**
 * @brief Updates the screen with the content rendered on the back buffer
 *
 */
void Draw();

namespace RendererInfo {
int MaxTextureUnits();
}  // namespace RendererInfo
}  // namespace verna

#endif