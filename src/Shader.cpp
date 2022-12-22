#include <viverna/core/Debug.hpp>
#include <viverna/graphics/GraphicsAPIHelper.hpp>
#include <viverna/graphics/Shader.hpp>

#include <array>
#include <string>

#if defined(VERNA_ANDROID)
#define VERNA_SHADER_VERSION_STRING "#version 320 es\n"
#define VERNA_SHADER_FRAGMENT_PRECISION "precision mediump float;\n"
#define VERNA_SHADER_VERTEX_PREFIX VERNA_SHADER_VERSION_STRING
#define VERNA_SHADER_FRAGMENT_PREFIX \
    VERNA_SHADER_VERSION_STRING VERNA_SHADER_FRAGMENT_PRECISION
#elif defined(VERNA_DESKTOP)
#define VERNA_SHADER_VERSION_STRING "#version 460 core\n"
#define VERNA_SHADER_VERTEX_PREFIX VERNA_SHADER_VERSION_STRING
#define VERNA_SHADER_FRAGMENT_PREFIX VERNA_SHADER_VERSION_STRING
#endif

namespace verna {

ShaderId LoadShaderFromSource(std::string_view vertex_src,
                              std::string_view fragment_src) {
    GLuint vertex, fragment, program;
    GLint success;
    std::array<GLchar, 256> log;
    GLsizei loglen;

    std::array<const char*, 2> source;
    std::array<GLint, source.size()> lengths;
    source[0] = VERNA_SHADER_VERTEX_PREFIX;
    lengths[0] = std::string_view(source[0]).length();
    source[1] = vertex_src.data();
    lengths[1] = vertex_src.length();
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, source.size(), source.data(), lengths.data());
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(vertex, log.size(), &loglen, log.data());
        VERNA_LOGE("Vertex shader compilation failed: "
                   + std::string(log.data(), loglen));
        glDeleteShader(vertex);
        return VERNA_INVALID_SHADER;
    }

    source[0] = VERNA_SHADER_FRAGMENT_PREFIX;
    lengths[0] = std::string_view(source[0]).length();
    source[1] = fragment_src.data();
    lengths[1] = fragment_src.length();
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, source.size(), source.data(), lengths.data());
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(fragment, log.size(), &loglen, log.data());
        VERNA_LOGE("Fragment shader compilation failed: "
                   + std::string(log.data(), loglen));
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return VERNA_INVALID_SHADER;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(program, log.size(), &loglen, log.data());
        VERNA_LOGE("Shader program linking failed: "
                   + std::string(log.data(), loglen));
        glDeleteProgram(program);
        return VERNA_INVALID_SHADER;
    }
    return program;
}

void FreeShader(ShaderId shader_program) {
    glDeleteProgram(shader_program);
}

}  // namespace verna