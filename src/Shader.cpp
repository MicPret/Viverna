#include <viverna/graphics/Shader.hpp>
#include <viverna/graphics/GraphicsAPIHelper.hpp>
#include <viverna/core/Debug.hpp>

#include <array>
#include <string>

#if defined(VERNA_ANDROID)
#define VERNA_SHADER_VERSION_STRING "#version 320 es\n\n"
#elif defined(VERNA_DESKTOP)
#define VERNA_SHADER_VERSION_STRING "#version 460 core\n\n"
#endif

namespace verna {

ShaderId LoadShaderFromSource(std::string_view vertex_src, std::string_view fragment_src) {
    GLuint vertex, fragment, program;
    GLint success;
    std::array<GLchar, 256> log;
    GLsizei loglen;

    std::array<const char*, 2> source = { VERNA_SHADER_VERSION_STRING, nullptr };
    std::array<GLint, source.size()> lengths = {
        std::string_view(VERNA_SHADER_VERSION_STRING).length(),  0 };

    vertex = glCreateShader(GL_VERTEX_SHADER);
    source[1] = vertex_src.data();
    lengths[1] = vertex_src.length();
    glShaderSource(vertex, source.size(), source.data(), lengths.data());
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(vertex, log.size(), &loglen, log.data());
        VERNA_LOGE("Vertex shader compilation failed: " + std::string(log.data(), loglen));
        glDeleteShader(vertex);
        return VERNA_INVALID_SHADER;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    source[1] = fragment_src.data();
    lengths[1] = fragment_src.length();
    glShaderSource(fragment, source.size(), source.data(), lengths.data());
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(fragment, log.size(), &loglen, log.data());
        VERNA_LOGE("Fragment shader compilation failed: " + std::string(log.data(), loglen));
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
        VERNA_LOGE("Shader program linking failed: " + std::string(log.data(), loglen));
        glDeleteProgram(program);
        return VERNA_INVALID_SHADER;
    }
    return program;
}

void FreeShader(ShaderId shader_program) {
    glDeleteProgram(shader_program);
}

}