#ifdef VERNA_DESKTOP
#line 3
#define DRAW_ID int(v_in[0].MESH_IDX)
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
v_in[];

out VS_OUT {
    float MESH_IDX;
    vec3 position;
    vec2 tex_coords;
    vec3 normal;
}
g_out;

void SetMeshIdx() {
    g_out.MESH_IDX = float(DRAW_ID);
}
