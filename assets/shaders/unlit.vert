layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoords;
out vec2 texCoords;
void main() {
    SetMeshIdx();
    texCoords = vTexCoords;
    gl_Position = camera.pv_matrix * MODEL_MATRIX
                  * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
}