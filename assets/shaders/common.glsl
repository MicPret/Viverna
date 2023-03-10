#ifdef VERNA_ANDROID
precision highp float;
precision highp int;
#endif

uniform sampler2D _textures[MAX_MATERIAL_TEXTURES];
uniform samplerCube _depth_maps[MAX_POINT_LIGHTS];

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
};
struct PointLightData {
    vec4 position;
    vec4 ambient_constant;
    vec4 diffuse_linear;
    vec4 specular_quadratic;
    mat4 view_matrices[6];
};

layout(std140) uniform FrameData {
    CameraData camera;
    PointLightData point_lights[MAX_POINT_LIGHTS];
    int num_point_lights;
};
layout(std140) uniform DrawData {
    MeshData draw_data[MAX_MESHES];
};
#define TEXTURE0 _textures[draw_data[DRAW_ID].texture_idx0]
#define TEXTURE1 _textures[draw_data[DRAW_ID].texture_idx1]
#define TEXTURE2 _textures[draw_data[DRAW_ID].texture_idx2]
#define TEXTURE3 _textures[draw_data[DRAW_ID].texture_idx3]
#define TEXTURE4 _textures[draw_data[DRAW_ID].texture_idx4]
#define TEXTURE5 _textures[draw_data[DRAW_ID].texture_idx5]
#define TEXTURE6 _textures[draw_data[DRAW_ID].texture_idx6]
#define TEXTURE7 _textures[draw_data[DRAW_ID].texture_idx7]
#define MATERIAL_PARAM0 draw_data[DRAW_ID].param0
#define MATERIAL_PARAM1 draw_data[DRAW_ID].param1
#define MATERIAL_PARAM2 draw_data[DRAW_ID].param2
#define MATERIAL_PARAM3 draw_data[DRAW_ID].param3
#define MODEL_MATRIX draw_data[DRAW_ID].model
#define TRANSPOSE_INVERSE_MODEL_MATRIX \
    draw_data[DRAW_ID].transpose_inverse_model
