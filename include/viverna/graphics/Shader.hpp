#ifndef VERNA_SHADER_HPP
#define VERNA_SHADER_HPP

#include <cstdint>
#include <string_view>

namespace verna {

using ShaderId = uint32_t;
constexpr ShaderId VERNA_INVALID_SHADER = 0;

/** Returns VERNA_INVALID_SHADER on failure */
ShaderId LoadShaderFromSource(std::string_view vertex_src,
                              std::string_view fragment_src);
/** Must be called for every loaded shader; FreeShader on VERNA_INVALID_SHADER
 * is a nop */
void FreeShader(ShaderId shader_program);

}  // namespace verna

#endif