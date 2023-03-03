out vec3 v_position;

void main() {
    SetMeshIdx();
    v_tex_coords = in_tex_coords;
    v_normal = mat3(TRANSPOSE_INVERSE_MODEL_MATRIX) * in_normal;

    vec4 pos = MODEL_MATRIX * vec4(in_position, 1.0);
    v_position = vec3(pos);
    gl_Position = camera.pv_matrix * pos;
}