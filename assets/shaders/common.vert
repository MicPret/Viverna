#ifdef VERNA_DESKTOP
#line 3
#define DRAW_ID gl_DrawID
#endif
#ifdef VERNA_ANDROID
layout(location = 0) uniform int DRAW_ID;
#endif

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_tex_coords;
layout(location = 2) in vec3 in_normal;

out float MESH_IDX;
out vec2 v_tex_coords;
out vec3 v_normal;

void SetMeshIdx() {
    MESH_IDX = float(DRAW_ID);
}
