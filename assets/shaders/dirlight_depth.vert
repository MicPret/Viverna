void main() {
    // SetMeshIdx();
    vec4 pos =
        direction_light.pv_matrix * MODEL_MATRIX * vec4(in_position, 1.0);
    pos.w = (-2.0 * DOES_NOT_CAST_SHADOW + 1.0) * pos.w;
    gl_Position = pos;
}
