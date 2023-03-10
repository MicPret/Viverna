void main() {
    vec3 light_position = v.normal;
    float inverse_depth = v.tex_coords.x;

    float light_dist = distance(v.position, light_position);
    gl_FragDepth = light_dist * inverse_depth;
}
