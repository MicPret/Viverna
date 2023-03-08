void main() {
    SetMeshIdx();
    v.tex_coords = in_tex_coords;
    gl_Position = camera.pv_matrix * MODEL_MATRIX
                  * vec4(in_position.x, in_position.y, in_position.z, 1.0);
}
