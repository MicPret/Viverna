layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 light_projection;
uniform int light_id;

void main() {
    float m10 = light_projection[2][2];
    float m14 = light_projection[3][2];
    // This is (far - near)^-1
    float inverse_depth = (m10 * m10 - 1.0) / (2.0 * m14);
    // float far = -m14 / (m10 + 1.0)
    // float near = -m14 / (m10 - 1.0)
    PointLightData light = point_lights[light_id];
    for (int face = 0; face < 6; ++face) {
        gl_Layer = face;
        for (int i = 0; i < 3; ++i) {
            SetMeshIdx();
            vec4 pos = MODEL_MATRIX * gl_in[i].gl_Position;
            g_out.position = vec3(pos);
            // tex_coords is unused, so I use it to store inverse_depth
            g_out.tex_coords.x = inverse_depth;
            // normal is unused, so I use it to store light.position
            g_out.normal = vec3(light.position);
            gl_Position = light_projection * light.view_matrices[face] * pos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
