#ifndef VERNA_SHADER_MANAGER_HPP
#define VERNA_SHADER_MANAGER_HPP

#include "Shader.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace verna {
class ShaderManager {
   public:
    ShaderManager() = default;
    ~ShaderManager();
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager(ShaderManager&&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;
    ShaderManager& operator=(ShaderManager&&) = delete;
    /**
     * @brief Compiles a shader program from shader files with the given name
     * (e.g. LoadShader("unlit") will compile and link "shaders/unlit.vert" and
     * "shaders/unlit.frag" from the assets folder)
     *
     * @param shader_name Name of shader files (with different extensions)
     * @return Shader identifier, must be freed with FreeShader
     */
    ShaderId LoadShader(std::string_view shader_name);
    /**
     * @brief Compiles a shader program from source. Every shader must be freed
     * with verna::FreeShader(verna::ShaderId)
     *
     * @param new_name Name that will be assigned to the generated shader
     * program
     * @param vertex_src GLSL code for the vertex shader. Must not specify GLSL
     * version
     * @param fragment_src  GLSL code for the fragment shader. Must not specify
     * GLSL version or floating point precision
     * @return Shader identifier which will be invalid on failure
     */
    ShaderId LoadShaderFromSource(std::string_view new_name,
                                  std::string_view vertex_src,
                                  std::string_view fragment_src);
    /**
     * @brief Compiles a shader program from source. Every shader must be freed
     * with verna::FreeShader(verna::ShaderId)
     *
     * @param new_name Name that will be assigned to the generated shader
     * program
     * @param vertex_src GLSL code for the vertex shader. Must not specify GLSL
     * version
     * @param geometry_src GLSL code for the geometry shader. Must not specify
     * GLSL version
     * @param fragment_src  GLSL code for the fragment shader. Must not specify
     * GLSL version or floating point precision
     * @return Shader identifier which will be invalid on failure
     */
    ShaderId LoadShaderFromSource(std::string_view new_name,
                                  std::string_view vertex_src,
                                  std::string_view geomatry_src,
                                  std::string_view fragment_src);
    /**
     * @brief Frees a shader, must be called for every loaded shader. Calling
     * FreeShader(ShaderId) on an invalid shader is a no-op
     *
     * @param shader_program Shader id returned by a LoadShader... function
     */
    void FreeShader(ShaderId shader_program);
    void FreeLoadedShaders();
    /**
     * @brief Retrieves the argument of LoadShader. Won't work if
     * FreeShader(shader_program) was called
     *
     * @param shader_program Shader returned by LoadShader
     * @return Argument passed to LoadShader
     */
    std::string GetShaderName(ShaderId shader_program) const;

   private:
    std::vector<ShaderId> shaders;
    std::vector<std::string> names;
};
}  // namespace verna

#endif
