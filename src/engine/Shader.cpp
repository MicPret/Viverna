#include <viverna/graphics/Shader.hpp>
#include <viverna/core/Assets.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/graphics/Renderer.hpp>
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

#ifndef NDEBUG
static ResourceTracker<ShaderId::id_type> shader_tracker("Shader");
#endif

static std::string ShaderPreface() {
    std::string max_meshes = std::to_string(gpu::DrawData::MAX_MESHES);
    std::string max_textures = std::to_string(RendererInfo::MaxTextureUnits());
    return
#if defined(VERNA_DESKTOP)
        "#version 460 core\n"
        "#define VERNA_DESKTOP 1\n\n"
#elif defined(VERNA_ANDROID)
        "#version 320 es\n"
        "#define VERNA_ANDROID 1\n\n"
#else
#error Platform not supported!
#endif
        "#define MAX_MESHES "
        + max_meshes + "\n#define MAX_TEXTURES " + max_textures + "\n\n";
}

static std::string ShaderCommonCode(GLenum shader_type) {
    std::string temp_path = "shaders/";
    switch (shader_type) {
        case GL_VERTEX_SHADER:
            temp_path += "common.vert";
            break;
        case GL_FRAGMENT_SHADER:
            temp_path += "common.frag";
            break;
        default:
            VERNA_LOGE("ShaderCommonCode failed: unsupported shader type!");
            return "";
            break;
    }
    auto path = std::filesystem::path(temp_path).make_preferred();
    auto common_code_raw = LoadRawAsset(path);
    if (common_code_raw.empty()) {
        VERNA_LOGE("ShaderCommonCode failed: can't load " + path.string());
        return "";
    }
    return std::string(common_code_raw.data(), common_code_raw.size());
}

static bool CompileShaderSources(const std::vector<std::string_view>& sources,
                                 const std::vector<GLenum>& shader_types,
                                 std::vector<GLuint>& output) {
    size_t size = shader_types.size();
    output.resize(size, 0);
    std::vector<std::string> common_sources(size);
    for (size_t i = 0; i < size; i++) {
        constexpr size_t num = 2;
        std::array<std::string, num> gl_sources;
        std::array<const char*, num> gl_sources_ptr;
        std::array<GLint, num> gl_sources_len;
        gl_sources[0] = ShaderPreface() + ShaderCommonCode(shader_types[i]);
        gl_sources[1] = std::string(sources[i]);

        for (size_t j = 0; j < num; j++) {
            gl_sources_ptr[j] = gl_sources[j].data();
            gl_sources_len[j] = gl_sources[j].length();
        }

        output[i] = glCreateShader(shader_types[i]);
        glShaderSource(output[i], gl_sources_ptr.size(), gl_sources_ptr.data(),
                       gl_sources_len.data());
        glCompileShader(output[i]);
        GLint success;
        std::array<GLchar, 256> log;
        GLsizei loglen;
        glGetShaderiv(output[i], GL_COMPILE_STATUS, &success);
        if (success == GL_TRUE)
            continue;
        glGetShaderInfoLog(output[i], log.size(), &loglen, log.data());
        VERNA_LOGE("Shader compilation failed: "
                   + std::string(log.data(), loglen));
        return false;
    }
    return true;
}

static bool LinkShaders(const std::vector<GLuint>& shaders,
                        GLuint& output_program) {
    output_program = glCreateProgram();
    for (size_t i = 0; i < shaders.size(); i++)
        glAttachShader(output_program, shaders[i]);
    glLinkProgram(output_program);
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

static void UniformInit(GLuint program) {
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
    GLuint program = 0u;
    constexpr GLuint no_shader = 0u;
    std::vector<GLuint> shaders(2, no_shader);
    std::vector<std::string_view> sources = {vertex_src, fragment_src};
    std::vector<GLenum> types = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
    if (CompileShaderSources(sources, types, shaders)) {
        if (!LinkShaders(shaders, program)) {
            program = 0u;
            VERNA_LOGE("LoadShaderFromSource failed: linking failed!");
        } else {
            glUseProgram(program);
            UniformInit(program);
#ifndef NDEBUG
            shader_tracker.Push(program);
#endif
        }
    }
    for (auto shader : shaders)
        glDeleteShader(shader);
    return ShaderId(program);
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