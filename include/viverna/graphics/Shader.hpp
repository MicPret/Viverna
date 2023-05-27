#ifndef VERNA_SHADER_HPP
#define VERNA_SHADER_HPP

#include <cstdint>
#include <string_view>
#include <filesystem>

namespace verna {

/**
 * @brief Shader program identifier
 *
 */
struct ShaderId {
    using id_type = uint32_t;
    id_type id;
    constexpr ShaderId() : id(0u) {}
    constexpr ShaderId(id_type id_) : id(id_) {}
    constexpr bool IsValid() const { return id != 0; }
};
constexpr bool operator==(ShaderId a, ShaderId b) {
    return a.id == b.id;
}
constexpr bool operator!=(ShaderId a, ShaderId b) {
    return !(a == b);
}

// /**
//  * @brief Compiles a shader program from shader files with the given name
//  (e.g.
//  * LoadShader("unlit") will compile and link shaders/unlit.vert and
//  * shaders/unlit.frag from the assets folder)
//  *
//  * @param shader_name Name of shader files (with different extensions)
//  * @return Shader identifier, must be freed with FreeShader
//  */
// ShaderId LoadShader(std::string_view shader_name);

// /**
//  * @brief Compiles a shader program from source. Every shader must be freed
//  with
//  * verna::FreeShader(verna::ShaderId)
//  *
//  * @param vertex_src GLSL code for the vertex shader. Must not specify GLSL
//  * version
//  * @param fragment_src  GLSL code for the fragment shader. Must not specify
//  GLSL
//  * version or floating point precision
//  * @return Shader identifier which will be invalid on failure
//  */
// ShaderId LoadShaderFromSource(std::string_view vertex_src,
//                               std::string_view fragment_src);

// /**
//  * @brief Frees a shader, must be called for every loaded shader. Calling
//  * FreeShader(ShaderId) on an invalid shader is a no-op
//  *
//  * @param shader_program Shader id returned by a LoadShader... function
//  */
// void FreeShader(ShaderId shader_program);

// /**
//  * @brief Retrieves the argument of LoadShader. Won't work if
//  * FreeShader(shader_program) was called
//  *
//  * @param shader_program Shader returned by LoadShader
//  * @return Argument passed to LoadShader
//  */
// std::string GetShaderName(ShaderId shader_program);

// int32_t GetShaderUniformLocation(ShaderId shader_program,
//                                  std::string_view uniform_name);

// struct Mat4f;
// void SetShaderUniform(ShaderId shader_program,
//                       const Mat4f& matrix,
//                       bool transpose = false);
}  // namespace verna

#endif