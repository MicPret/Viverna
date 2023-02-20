#include <viverna/graphics/Shader.hpp>
#include <viverna/core/Assets.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/ShaderCommonCode.hpp>
#include <viverna/graphics/gpu/FrameData.hpp>
#include <viverna/graphics/gpu/DrawData.hpp>
#include <viverna/maths/Mat4f.hpp>
#include "ResourceTracker.hpp"

#if defined(VERNA_DESKTOP)
#include <glad/gl.h>
#elif defined(VERNA_ANDROID)
#include <GLES3/gl32.h>
#else
#error Platform not supported!
#endif

#include <array>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

namespace verna {

namespace {
#ifndef NDEBUG
ResourceTracker<ShaderId::id_type> shader_tracker("Shader");
#endif

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
            VERNA_LOGE(
                "CompileShaderSource() failed: unsupported shader type!");
            return false;
    }
    sources.push_back(std::string(source));
    for (const std::string& s : sources) {
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
    glUseProgram(program);
    GLuint block_loc =
        glGetUniformBlockIndex(program, gpu::FrameData::BLOCK_NAME);
    glUniformBlockBinding(program, block_loc, gpu::FrameData::BLOCK_BINDING);
    block_loc = glGetUniformBlockIndex(program, gpu::DrawData::BLOCK_NAME);
    glUniformBlockBinding(program, block_loc, gpu::DrawData::BLOCK_BINDING);

    constexpr GLint textures_uniform_loc = 1;
    GLint textures_loc = glGetUniformLocation(program, "_textures[0]");
    if (textures_uniform_loc != textures_loc)  // textures optimized out
        return;
    std::vector<GLint> texture_slots(RendererInfo::MaxTextureUnits());
    GLsizei count = static_cast<GLsizei>(texture_slots.size());
    for (GLint i = 0; i < count; i++)
        texture_slots[i] = i;
    glUniform1iv(textures_uniform_loc, count, texture_slots.data());
}
}  // namespace

ShaderId LoadShader(std::string_view shader_name) {
    std::filesystem::path path = std::filesystem::path("shaders") / shader_name;
    std::filesystem::path vertex_path = path.string() + ".vert";
    std::filesystem::path fragment_path = path.string() + ".frag";
    auto vertex_raw = LoadRawAsset(vertex_path);
    if (vertex_raw.empty()) {
        VERNA_LOGE("LoadShader failed: can't load " + vertex_path.string());
        return ShaderId();
    }
    auto fragment_raw = LoadRawAsset(fragment_path);
    if (fragment_raw.empty()) {
        VERNA_LOGE("LoadShader failed: can't load " + fragment_path.string());
        return ShaderId();
    }
    std::string_view vertex_src(vertex_raw.data(), vertex_raw.size());
    std::string_view fragment_src(fragment_raw.data(), fragment_raw.size());
    return LoadShaderFromSource(vertex_src, fragment_src);
}

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
#ifndef NDEBUG
    shader_tracker.Push(program);
#endif
    output.id = program;
    return output;
}

void FreeShader(ShaderId shader_program) {
    if (!shader_program.IsValid())
        return;
    glDeleteProgram(shader_program.id);
#ifndef NDEBUG
    shader_tracker.Remove(shader_program.id);
#endif
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