#ifdef VERNA_ANDROID
precision highp float;
precision highp int;
#endif

uniform sampler2D material_textures[MAX_MATERIAL_TEXTURES];
uniform sampler2D dirlight_depthmap;

struct MeshData {
    int texture_idx0;
    int texture_idx1;
    int texture_idx2;
    int texture_idx3;
    int texture_idx4;
    int texture_idx5;
    int texture_idx6;
    int texture_idx7;
    float param0;
    float param1;
    float param2;
    float param3;
    mat4 model;
    mat4 transpose_inverse_model;
};
struct CameraData {
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 pv_matrix;
    float near;
    float far;
    float aspect_ratio;
    float fovy;
};
struct DirectionLightData {
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    mat4 pv_matrix;
};

layout(std140) uniform FrameData {
    CameraData camera;
    DirectionLightData direction_light;
};
layout(std140) uniform DrawData {
    MeshData draw_data[MAX_MESHES];
};
#define TEXTURE0 material_textures[draw_data[DRAW_ID].texture_idx0]
#define TEXTURE1 material_textures[draw_data[DRAW_ID].texture_idx1]
#define TEXTURE2 material_textures[draw_data[DRAW_ID].texture_idx2]
#define TEXTURE3 material_textures[draw_data[DRAW_ID].texture_idx3]
#define TEXTURE4 material_textures[draw_data[DRAW_ID].texture_idx4]
#define TEXTURE5 material_textures[draw_data[DRAW_ID].texture_idx5]
#define TEXTURE6 material_textures[draw_data[DRAW_ID].texture_idx6]
#define TEXTURE7 material_textures[draw_data[DRAW_ID].texture_idx7]
#define MATERIAL_PARAM0 draw_data[DRAW_ID].param0
#define MATERIAL_PARAM1 draw_data[DRAW_ID].param1
#define MATERIAL_PARAM2 draw_data[DRAW_ID].param2
#define MATERIAL_PARAM3 draw_data[DRAW_ID].param3
#define MODEL_MATRIX draw_data[DRAW_ID].model
#define TRANSPOSE_INVERSE_MODEL_MATRIX \
    draw_data[DRAW_ID].transpose_inverse_model

#define DOES_NOT_CAST_SHADOW MATERIAL_PARAM3

vec3 GetCameraPosition() {
    vec3 p;
    p.x = camera.view_matrix[3][0];
    p.y = camera.view_matrix[3][1];
    p.z = camera.view_matrix[3][2];
    return p;
}