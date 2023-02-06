#include <viverna/graphics/ShaderCommonCode.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/gpu/DrawData.hpp>

#if defined(VERNA_DESKTOP)
#include <glad/gl.h>
#elif defined(VERNA_ANDROID)
#include <GLES3/gl32.h>
#else
#error Platform not supported!
#endif

#include <string>
#include <string_view>

namespace {
struct Code {
    std::string code;
    void AddDefine(std::string_view define_name, std::string_view define_value);
    void AddLine(std::string_view line);
};

inline void Code::AddDefine(std::string_view define_name,
                            std::string_view define_value) {
    code.append("#define ")
        .append(define_name)
        .append(" ")
        .append(define_value)
        .append("\n");
}
inline void Code::AddLine(std::string_view line) {
    code.append(line).append("\n");
}

std::string InternalVertexCommonCode() {
    Code vertex;
#if defined(VERNA_DESKTOP)
    vertex.AddLine("#version 460 core");
    vertex.AddDefine("DRAW_ID", "gl_DrawID");
#elif defined(VERNA_ANDROID)
    vertex.AddLine("#version 320 es");
    vertex.AddLine("uniform int DRAW_ID;");
#endif
    vertex.AddLine("out float MESH_IDX;");
    vertex.AddLine("void SetMeshIdx() { MESH_IDX = float(DRAW_ID); }");
    return vertex.code;
}

std::string InternalFragmentCommonCode() {
    Code fragment;
#if defined(VERNA_DESKTOP)
    fragment.AddLine("#version 460 core");
    fragment.AddDefine("DRAW_ID", "int(MESH_IDX)");
#elif defined(VERNA_ANDROID)
    fragment.AddLine("#version 320 es");
    fragment.AddLine("precision mediump float;");
    fragment.AddLine("precision highp int;");
    fragment.AddLine("uniform int DRAW_ID;");
#endif
    fragment.AddLine("in float MESH_IDX;");
    return fragment.code;
}

std::string InternalLaterCommonCode() {
    Code commons;
    commons.AddDefine("MAX_MESHES",
                      std::to_string(verna::gpu::DrawData::MAX_MESHES));
    commons.AddDefine("MAX_TEXTURES",
                      std::to_string(verna::RendererInfo::MaxTextureUnits()));
    commons.AddLine(
        "struct MeshData {\n"
        "  int texture_idx0;\n"
        "  int texture_idx1;\n"
        "  int texture_idx2;\n"
        "  int texture_idx3;\n"
        "  int texture_idx4;\n"
        "  int texture_idx5;\n"
        "  int texture_idx6;\n"
        "  int texture_idx7;\n"
        "  float param0;\n"
        "  float param1;\n"
        "  float param2;\n"
        "  float param3;\n"
        "  mat4 model;\n"
        "};\n"
        "struct CameraData {\n"
        "  mat4 projection_matrix;\n"
        "  mat4 view_matrix;\n"
        "  mat4 pv_matrix;\n"
        "};\n"
        "layout(std140) uniform FrameData {\n"
        "  CameraData camera;\n"
        "};\n"
        "layout(std140) uniform DrawData {\n"
        "  MeshData draw_data[MAX_MESHES];\n"
        "};\n"
        "uniform sampler2D _textures[MAX_TEXTURES];\n");
    commons.AddDefine("TEXTURE0", "_textures[draw_data[DRAW_ID].texture_idx0]");
    commons.AddDefine("TEXTURE1", "_textures[draw_data[DRAW_ID].texture_idx1]");
    commons.AddDefine("TEXTURE2", "_textures[draw_data[DRAW_ID].texture_idx2]");
    commons.AddDefine("TEXTURE3", "_textures[draw_data[DRAW_ID].texture_idx3]");
    commons.AddDefine("TEXTURE4", "_textures[draw_data[DRAW_ID].texture_idx4]");
    commons.AddDefine("TEXTURE5", "_textures[draw_data[DRAW_ID].texture_idx5]");
    commons.AddDefine("TEXTURE6", "_textures[draw_data[DRAW_ID].texture_idx6]");
    commons.AddDefine("TEXTURE7", "_textures[draw_data[DRAW_ID].texture_idx7]");
    commons.AddDefine("MODEL_MATRIX", "draw_data[DRAW_ID].model");
    return commons.code;
}

}  // namespace
namespace verna {
std::string GetCommonVertexCode() {
    return InternalVertexCommonCode() + InternalLaterCommonCode();
}
std::string GetCommonFragmentCode() {
    return InternalFragmentCommonCode() + InternalLaterCommonCode();
}
}  // namespace verna