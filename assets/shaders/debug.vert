void main() {
    SetMeshIdx();
    gl_Position = camera.pv_matrix * vec4(in_position, 1.0);
}
