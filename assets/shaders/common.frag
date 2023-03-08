#ifdef VERNA_DESKTOP
#line 3
#define DRAW_ID int(v.MESH_IDX)
#endif
#ifdef VERNA_ANDROID
uniform int DRAW_ID;
#endif

in VS_OUT {
    float MESH_IDX;
    vec3 position;
    vec2 tex_coords;
    vec3 normal;
}
v;

out vec4 f_color;
