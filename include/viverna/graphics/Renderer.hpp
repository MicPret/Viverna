#ifndef VERNA_RENDERER_HPP
#define VERNA_RENDERER_HPP

#include <viverna/maths/Mat4f.hpp>
#include "Mesh.hpp"
#include "Shader.hpp"

namespace verna {
void InitializeRenderer();
void TerminateRenderer();
void Render(const Mesh& mesh,
            const Mat4f& transform_matrix,
            ShaderId shader_id);
void Draw();
}  // namespace verna

#endif