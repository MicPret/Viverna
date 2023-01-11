#include <viverna/core/Debug.hpp>
#include <viverna/graphics/Shader.hpp>
#include <viverna/graphics/ShaderCommonCode.hpp>
#include <viverna/graphics/gpu/CameraData.hpp>
#include <viverna/graphics/gpu/MeshData.hpp>
#include <viverna/maths/Mat4f.hpp>
#include "GraphicsAPIHelper.hpp"
#include "ResourceTracker.hpp"

#include <array>
#include <string>
#include <vector>

namespace verna {

namespace {
class ResourceTracker<ShaderId::id_type> shader_tracker("Shader");

bool CompileShaderSource(std::string_view source,
                         GLenum shader_type,
                         GLuint& output) {
    std::vector<std::string> sources;
    std::vector<const char*> sources_ptr;
    std::vector<GLint> sources_len;

    switch (shader_type) {
        case GL_VERTEX_SHADER:
            sources.push_back(GetCommonVertexCode());
            break;
        case GL_FRAGMENT_SHADER:
            sources.push_back(GetCommonFragmentCode());
            break;
        default:
            VERNA_LOGE("CompileShaderSource(...) failed: invalid shader_type!");
            return false;
    }
    sources.push_back(std::string(source));
    for (const std::string& s : sources) {
#ifndef NDEBUG
        VERNA_LOGI(s);
#endif
        sources_ptr.push_back(s.data());
        sources_len.push_back(s.length());
    }

    output = glCreateShader(shader_type);
    glShaderSource(output, sources_ptr.size(), sources_ptr.data(),
                   sources_len.data());
    glCompileShader(output);
    GLint success;
    std::array<GLchar, 256> log;
    GLsizei loglen;
    glGetShaderiv(output, GL_COMPILE_STATUS, &success);
    if (success == GL_TRUE)
        return true;
    glGetShaderInfoLog(output, log.size(), &loglen, log.data());
    VERNA_LOGE("Vertex shader compilation failed: "
               + std::string(log.data(), loglen));
    glDeleteShader(output);
    return false;
}

bool LinkShaders(GLuint vertex_shader,
                 GLuint fragment_shader,
                 GLuint& output_program) {
    output_program = glCreateProgram();
    glAttachShader(output_program, vertex_shader);
    glAttachShader(output_program, fragment_shader);
    glLinkProgram(output_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    GLint success;
    std::array<GLchar, 256> log;
    GLsizei loglen;
    glGetProgramiv(output_program, GL_LINK_STATUS, &success);
    if (success == GL_TRUE)
        return true;
    glGetProgramInfoLog(output_program, log.size(), &loglen, log.data());
    VERNA_LOGE("Shader program linking failed: "
               + std::string(log.data(), loglen));
    glDeleteProgram(output_program);
    return false;
}

void UniformInit(GLuint program) {
    GLuint block_loc =
        glGetUniformBlockIndex(program, gpu::CameraData::BLOCK_NAME);
    glUniformBlockBinding(program, block_loc, gpu::CameraData::BLOCK_BINDING);
    block_loc =
        glGetUniformBlockIndex(program, gpu::MeshDataBuffer::BLOCK_NAME);
    glUniformBlockBinding(program, block_loc,
                          gpu::MeshDataBuffer::BLOCK_BINDING);

    for (int i = 0; i < gpu::MaxTextureUnits(); i++) {
        std::string texture_uniform_name =
            "_TEXTURES[" + std::to_string(i) + "]";
        GLint textures_loc =
            glGetUniformLocation(program, texture_uniform_name.c_str());
        glUniform1i(textures_loc, i);
    }
}
}  // namespace

ShaderId LoadShaderFromSource(std::string_view vertex_src,
                              std::string_view fragment_src) {
    GLuint vertex, fragment, program;
    ShaderId output;  // default constructor makes it invalid

    if (!CompileShaderSource(vertex_src, GL_VERTEX_SHADER, vertex)) {
        return output;
    }
    if (!CompileShaderSource(fragment_src, GL_FRAGMENT_SHADER, fragment)) {
        glDeleteShader(vertex);
        return output;
    }
    if (!LinkShaders(vertex, fragment, program)) {
        return output;
    }

    glUseProgram(program);
    UniformInit(program);

    shader_tracker.Push(program);
    output.id = program;
    return output;
}

void FreeShader(ShaderId shader_program) {
    if (!shader_program.IsValid())
        return;
    glDeleteProgram(shader_program.id);
    shader_tracker.Remove(shader_program.id);
}

int32_t GetShaderUniformLocation(ShaderId shader_program,
                                 std::string_view uniform_name) {
    glUseProgram(shader_program.id);
    return glGetUniformLocation(shader_program.id, uniform_name.data());
}

void SetShaderUniform(ShaderId shader_program,
                      int32_t location,
                      const Mat4f& matrix,
                      bool transpose) {
    glUseProgram(shader_program.id);
    glUniformMatrix4fv(location, 1, transpose ? GL_TRUE : GL_FALSE,
                       matrix.raw.data());
}

}  // namespace verna