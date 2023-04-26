out vec4 lightspace_pos;

void main() {
    SetMeshIdx();
    v.tex_coords = in_tex_coords;
    v.normal = mat3(TRANSPOSE_INVERSE_MODEL_MATRIX) * in_normal;

    vec4 pos = MODEL_MATRIX * vec4(in_position, 1.0);
    v.position = vec3(pos);
    lightspace_pos = direction_light.pv_matrix * pos;
    gl_Position = camera.pv_matrix * pos;
}