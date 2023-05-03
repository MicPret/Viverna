#ifndef VERNA_RENDERER_HPP
#define VERNA_RENDERER_HPP

#include <viverna/core/BoundingBox.hpp>
#include <viverna/core/BoundingSphere.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/core/VivernaState.hpp>
#include <viverna/maths/Mat4f.hpp>
#include "Mesh.hpp"
#include "Model.hpp"
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
 * @brief Adds an element to the render batch
 *
 * @param mesh The mesh that must be rendered
 * @param material The material properties used to render the mesh
 * @param transform The transformation used to give the mesh a position,
 * rotation and scale
 * @param shader_id The identifier for the shader program to use to render the
 * mesh
 */
void Render(const Mesh& mesh,
            const Material& material,
            const Transform& transform,
            ShaderId shader_id);

void Render(const BoundingBox& box);
void Render(const BoundingSphere& sphere);

/**
 * @brief Draws the batch on the back buffer, which is not shown until
 * NextFrame() gets called
 *
 */
void Draw();

/**
 * @brief Swaps buffers and clears the new back buffer
 *
 */
void NextFrame();

namespace RendererInfo {
int MaxTextureUnits();
int MaxMaterialTextures();
}  // namespace RendererInfo
}  // namespace verna

#endif