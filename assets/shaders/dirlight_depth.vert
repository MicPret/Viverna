void main() {
    // SetMeshIdx();
    vec4 pos =
        direction_light.pv_matrix * MODEL_MATRIX * vec4(in_position, 1.0);
    pos.z = (DOES_NOT_CAST_SHADOW * -2.0 + 1.0) * pos.z;
    gl_Position = pos;
}
