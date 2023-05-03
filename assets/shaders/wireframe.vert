void main() {
    SetMeshIdx();
    gl_Position = camera.pv_matrix * MODEL_MATRIX * vec4(in_position, 1.0);
}
