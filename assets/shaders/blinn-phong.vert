out vec3 v_position;

void main() {
    SetMeshIdx();
    v_tex_coords = in_tex_coords;
    v_normal = mat3(TRANSPOSE_INVERSE_MODEL_MATRIX) * in_normal;

    mat4 view_model = camera.view_matrix * MODEL_MATRIX;
    vec4 pos = view_model * vec4(in_position, 1.0);
    v_position = vec3(pos) / pos.w;
    gl_Position = camera.projection_matrix * pos;
}