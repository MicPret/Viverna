#ifdef VERNA_DESKTOP
#line 3
#define DRAW_ID int(MESH_IDX)
#endif
#ifdef VERNA_ANDROID
uniform int DRAW_ID;
precision mediump float;
precision highp int;
#endif
uniform sampler2D _textures[MAX_TEXTURES];

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
};
struct CameraData {
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 pv_matrix;
};

layout(std140) uniform FrameData {
    CameraData camera;
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
#define MODEL_MATRIX draw_data[DRAW_ID].model

in float MESH_IDX;
in vec2 v_tex_coords;
out vec4 f_color;
