#include <viverna/graphics/ShaderManager.hpp>
#include <viverna/core/Assets.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/graphics/gpu/DrawData.hpp>
#include <viverna/graphics/gpu/FrameData.hpp>
#include <viverna/graphics/Renderer.hpp>

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

namespace verna {
static void CheckForGLErrors(std::string_view origin);
static std::string ShaderPreface();
static std::string ShaderCommonCode(GLenum shader_type);
static bool CompileShaderSources(const std::vector<std::string_view>& sources,
                                 const std::vector<GLenum>& shader_types,
                                 std::vector<GLuint>& output);
static bool LinkShaders(const std::vector<GLuint>& shaders,
                        GLuint& output_program);
static void UniformInit(GLuint program);
static ShaderId MakeProgramFromSource(
    const std::vector<std::string_view>& shader_sources,
    const std::vector<GLenum>& shader_types);

ShaderManager::~ShaderManager() {
    for (size_t i = 0; i < shaders.size(); i++)
        VERNA_LOGE("Shader not freed: " + names[i] + " ("
                   + std::to_string(shaders[i].id) + ')');
}

ShaderId ShaderManager::LoadShader(std::string_view shader_name) {
    for (size_t i = 0; i < names.size(); i++)
        if (names[i] == shader_name)
            return shaders[i];
    std::filesystem::path path = std::filesystem::path("shaders") / shader_name;
    std::filesystem::path vertex_path = path.string() + ".vert";
    std::filesystem::path fragment_path = path.string() + ".frag";
    std::filesystem::path geometry_path = path.string() + ".geom";

    std::vector<std::string_view> sources;
    std::vector<GLenum> shader_types;
    sources.reserve(3);
    shader_types.reserve(3);

    if (!AssetExists(vertex_path)) {
        VERNA_LOGE("LoadShader failed: can't find " + vertex_path.string());
        return ShaderId();
    }
    if (!AssetExists(fragment_path)) {
        VERNA_LOGE("LoadShader failed: can't find " + fragment_path.string());
        return ShaderId();
    }

    auto vertex_raw = LoadRawAsset(vertex_path);
    if (vertex_raw.empty()) {
        VERNA_LOGE("LoadShader failed: can't load " + vertex_path.string());
        return ShaderId();
    }
    sources.push_back(std::string_view(vertex_raw.data(), vertex_raw.size()));
    shader_types.push_back(GL_VERTEX_SHADER);
    auto fragment_raw = LoadRawAsset(fragment_path);
    if (fragment_raw.empty()) {
        VERNA_LOGE("LoadShader failed: can't load " + fragment_path.string());
        return ShaderId();
    }
    sources.push_back(
        std::string_view(fragment_raw.data(), fragment_raw.size()));
    shader_types.push_back(GL_FRAGMENT_SHADER);

    std::vector<char> geometry_raw;
    if (AssetExists(geometry_path)) {
        geometry_raw = LoadRawAsset(geometry_path);
        if (geometry_raw.empty()) {
            VERNA_LOGW(geometry_path.string()
                       + " found, but empty (or failed to load)");
        } else {
            sources.push_back(
                std::string_view(geometry_raw.data(), geometry_raw.size()));
            shader_types.push_back(GL_GEOMETRY_SHADER);
        }
    }

    ShaderId result = MakeProgramFromSource(sources, shader_types);
    if (result.IsValid()) {
        shaders.push_back(result);
        names.emplace_back(shader_name);
    }
    return result;
}

ShaderId ShaderManager::LoadShaderFromSource(std::string_view new_name,
                                             std::string_view vertex_src,
                                             std::string_view fragment_src) {
    std::vector sources = {vertex_src, fragment_src};
    std::vector<GLenum> types = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
    ShaderId result = MakeProgramFromSource(sources, types);
    if (result.IsValid()) {
        shaders.push_back(result);
        names.emplace_back(new_name);
    }
    return result;
}

ShaderId ShaderManager::LoadShaderFromSource(std::string_view new_name,
                                             std::string_view vertex_src,
                                             std::string_view geometry_src,
                                             std::string_view fragment_src) {
    std::vector sources = {vertex_src, geometry_src, fragment_src};
    std::vector<GLenum> types = {GL_VERTEX_SHADER, GL_GEOMETRY_SHADER,
                                 GL_FRAGMENT_SHADER};
    ShaderId result = MakeProgramFromSource(sources, types);
    if (result.IsValid()) {
        shaders.push_back(result);
        names.emplace_back(new_name);
    }
    return result;
}

void ShaderManager::FreeShader(ShaderId shader_program) {
    if (!shader_program.IsValid())
        return;
    for (size_t i = 0; i < shaders.size(); i++) {
        if (shaders[i] == shader_program) {
            shaders.erase(shaders.begin() + i);
            names.erase(names.begin() + i);
            glDeleteProgram(shader_program.id);
            return;
        }
    }
    VERNA_LOGI("Called FreeShader on missing shader: "
               + std::to_string(shader_program.id));
}

void ShaderManager::FreeLoadedShaders() {
    for (size_t i = 0; i < shaders.size(); i++) {
        glDeleteProgram(shaders[i].id);
    }
    shaders.clear();
    names.clear();
}

std::string ShaderManager::GetShaderName(ShaderId shader_program) const {
    for (size_t i = 0; i < shaders.size(); i++)
        if (shaders[i] == shader_program)
            return names[i];
    return {};
}

// static functions

std::string ShaderPreface() {
    std::string max_meshes = std::to_string(gpu::DrawData::MAX_MESHES);
    std::string max_material_textures =
        std::to_string(RendererInfo::MaxMaterialTextures());
    auto common_glsl_raw = LoadRawAsset("shaders/common.glsl");
    std::string common_glsl(common_glsl_raw.data(), common_glsl_raw.size());
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
        + max_meshes + "\n#define MAX_MATERIAL_TEXTURES "
        + max_material_textures + "\n\n" + common_glsl + "\n\n";
}

std::string ShaderCommonCode(GLenum shader_type) {
    std::string temp_path = "shaders/";
    switch (shader_type) {
        case GL_VERTEX_SHADER:
            temp_path += "common.vert";
            break;
        case GL_FRAGMENT_SHADER:
            temp_path += "common.frag";
            break;
        case GL_GEOMETRY_SHADER:
            temp_path += "common.geom";
            break;
        default:
            VERNA_LOGE("ShaderCommonCode failed: unsupported shader type!");
            return std::string();
    }
    auto path = std::filesystem::path(temp_path).make_preferred();
    auto common_code_raw = LoadRawAsset(path);
    if (common_code_raw.empty()) {
        VERNA_LOGE("ShaderCommonCode failed: can't load " + path.string());
        return std::string();
    }
    return std::string(common_code_raw.data(), common_code_raw.size());
}

bool CompileShaderSources(const std::vector<std::string_view>& sources,
                          const std::vector<GLenum>& shader_types,
                          std::vector<GLuint>& output) {
    size_t size = shader_types.size();
    output.resize(size, 0);
    for (size_t i = 0; i < size; i++) {
        constexpr size_t num = 3;
        std::array<std::string, num> gl_sources;
        std::array<const char*, num> gl_sources_ptr;
        std::array<GLint, num> gl_sources_len;
        gl_sources[0] = ShaderPreface();
        gl_sources[1] = ShaderCommonCode(shader_types[i]);
        gl_sources[2] = std::string(sources[i]);
#ifdef VERNA_PRINT_SHADER_COMMON_CODE
        static bool printed = false;
        if (!printed) {
            VERNA_LOGI(gl_sources[0] + gl_sources[1]);
            printed = true;
        }
#endif
        for (size_t j = 0; j < num; j++) {
            gl_sources_ptr[j] = gl_sources[j].data();
            gl_sources_len[j] = gl_sources[j].length();
        }

        output[i] = glCreateShader(shader_types[i]);
        CheckForGLErrors("CompileShaderSources->glCreateShader");
        glShaderSource(output[i], gl_sources_ptr.size(), gl_sources_ptr.data(),
                       gl_sources_len.data());
        CheckForGLErrors("CompileShaderSources->glShaderSource");
        glCompileShader(output[i]);
        GLint success;
        std::array<GLchar, 256> log;
        GLsizei loglen;
        glGetShaderiv(output[i], GL_COMPILE_STATUS, &success);
        if (success == GL_TRUE)
            continue;
        glGetShaderInfoLog(output[i], log.size(), &loglen, log.data());
        std::string shader_type_str;
        switch (shader_types[i]) {
            case GL_VERTEX_SHADER:
                shader_type_str = "Vertex";
                break;
            case GL_FRAGMENT_SHADER:
                shader_type_str = "Fragment";
                break;
            case GL_GEOMETRY_SHADER:
                shader_type_str = "Geometry";
                break;
            default:
                shader_type_str = "Unsupported";
                break;
        }
        VERNA_LOGE(shader_type_str + " shader compilation failed: "
                   + std::string(log.data(), loglen));
        return false;
    }
    return true;
}

bool LinkShaders(const std::vector<GLuint>& shaders, GLuint& output_program) {
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

void UniformInit(GLuint program) {
    glUseProgram(program);
    GLuint block_loc =
        glGetUniformBlockIndex(program, gpu::FrameData::BLOCK_NAME);
    glUniformBlockBinding(program, block_loc, gpu::FrameData::BLOCK_BINDING);
    block_loc = glGetUniformBlockIndex(program, gpu::DrawData::BLOCK_NAME);
    glUniformBlockBinding(program, block_loc, gpu::DrawData::BLOCK_BINDING);

    GLint textures_loc = glGetUniformLocation(program, "material_textures[0]");
    std::vector<GLint> texture_slots;
    GLsizei mat_texture_count =
        static_cast<GLsizei>(RendererInfo::MaxMaterialTextures());
    if (textures_loc != -1) {
        texture_slots.resize(mat_texture_count);
        for (GLint i = 0; i < mat_texture_count; i++)
            texture_slots[i] = i;
        glUniform1iv(textures_loc, mat_texture_count, texture_slots.data());
    }  // else materials optimized out
    textures_loc = glGetUniformLocation(program, "dirlight_depthmap");
    if (textures_loc != -1) {
        glUniform1i(textures_loc, static_cast<GLint>(mat_texture_count));
    }  // else direction light optimized out
}

ShaderId MakeProgramFromSource(
    const std::vector<std::string_view>& shader_sources,
    const std::vector<GLenum>& shader_types) {
    GLuint program = 0;
    std::vector<GLuint> shaders_out;

    if (CompileShaderSources(shader_sources, shader_types, shaders_out)) {
        if (!LinkShaders(shaders_out, program)) {
            program = 0;
            VERNA_LOGE("Shader linking failed!");
        } else {
            glUseProgram(program);
            UniformInit(program);
        }
    }
    for (GLuint s : shaders_out)
        glDeleteShader(s);
    return ShaderId(program);
}

void CheckForGLErrors(std::string_view origin) {
    GLenum glerr;
    while ((glerr = glGetError()) != GL_NO_ERROR) {
        std::string error_string;
        switch (glerr) {
            case GL_INVALID_ENUM:
                error_string = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error_string = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error_string = "GL_INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error_string = "GL_STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error_string = "GL_STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error_string = "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error_string = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                error_string = std::to_string(glerr);
                break;
        }
        VERNA_LOGE(std::string(origin)
                   + " detected OpenGL error: " + error_string);
    }
}

}  // namespace verna
