in vec2 texCoords;
out vec4 outputColor;
void main() {
    outputColor = texture(TEXTURE0, texCoords);
}