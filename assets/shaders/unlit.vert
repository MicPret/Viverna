void main() {
    SetMeshIdx();
    v_tex_coords = in_tex_coords;
    gl_Position = camera.pv_matrix * MODEL_MATRIX
                  * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
}
