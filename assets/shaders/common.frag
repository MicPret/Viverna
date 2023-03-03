#ifdef VERNA_DESKTOP
#line 3
#define DRAW_ID int(MESH_IDX)
#endif
#ifdef VERNA_ANDROID
uniform int DRAW_ID;
precision mediump float;
precision highp int;
#endif

in float MESH_IDX;
in vec2 v_tex_coords;
in vec3 v_normal;
out vec4 f_color;
