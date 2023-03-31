in vec4 lightspace_pos;

float CalculateShadow(vec2 depthmap_coords, float current_depth, float bias) {
    float shadow = texture(dirlight_depthmap, depthmap_coords).r;
    return (current_depth - bias) > shadow ? 1.0 : 0.0;
}

float CalculateShadowPCF(vec2 depthmap_coords,
                         float current_depth,
                         float bias) {
    if (current_depth > 1.0)
        return 0.0;
    float shadow = 0.0;
    ivec2 texture_size = textureSize(dirlight_depthmap, 0);
    float x_size = 1.0 / float(texture_size.x);
    float y_size = 1.0 / float(texture_size.y);
    for (int y = -1; y <= 1; y++) {
        float y_offs = float(y) * y_size;
        for (int x = -1; x <= 1; x++) {
            float x_offs = float(x) * x_size;
            vec2 offset = vec2(x_offs, y_offs);
            shadow +=
                CalculateShadow(depthmap_coords + offset, current_depth, bias);
        }
    }
    return shadow / 9.0;
}

vec3 CalculateLight(in vec3 to_light_normalized,
                    in vec3 to_eye_normalized,
                    in vec3 normal,
                    in vec3 light_amb,
                    in vec3 light_diff,
                    in vec3 light_spec,
                    in vec3 diff,
                    in vec3 spec,
                    in float shine) {
    vec3 ambient = light_amb * diff;
    float cosangle = dot(to_light_normalized, normal);

    vec3 halfway = normalize(to_light_normalized + to_eye_normalized);

    float diff_coeff = max(cosangle, 0.0);
    float spec_coeff = pow(max(dot(normal, halfway), 0.0), shine);

    vec3 diffuse = diff_coeff * light_diff * diff;
    vec3 specular = spec_coeff * light_spec * spec;

    // float bias = max(0.0003 * (1.0 - cosangle), 0.00003);
    float bias = 0.0;
    vec3 shadow_coords = (vec3(lightspace_pos) / lightspace_pos.w) * 0.5 + 0.5;
    float current_depth = shadow_coords.z;
    float shadow =
        current_depth > 1.0
            ? 0.0
            : CalculateShadowPCF(vec2(shadow_coords), shadow_coords.z, bias);

    // attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
    return ambient + (1.0 - shadow) * (diffuse + specular);
}

void main() {
    vec3 normal = normalize(v.normal);
    vec3 cam_pos = vec3(camera.view_matrix[3]);
    vec3 to_camera_normalized = normalize(cam_pos - v.position);

    vec3 diffuse_map = vec3(texture(DIFFUSE_TEXTURE, v.tex_coords));
    vec3 specular_map = vec3(texture(SPECULAR_TEXTURE, v.tex_coords));
    float shine;
#ifdef VERNA_ANDROID
    // Hack: when SHININESS is 0.0, it gives weird results
    shine = max(SHININESS, 0.0001);
#else
    shine = SHININESS;
#endif

    vec3 result = vec3(0.0);

    // Direction light
    vec3 light_dir = normalize(vec3(direction_light.direction));
    vec3 light_amb = vec3(direction_light.ambient);
    vec3 light_diff = vec3(direction_light.diffuse);
    vec3 light_spec = vec3(direction_light.specular);
    result += CalculateLight(-light_dir, to_camera_normalized, normal,
                             light_amb, light_diff, light_spec, diffuse_map,
                             specular_map, shine);

    f_color = vec4(result, 1.0);
}