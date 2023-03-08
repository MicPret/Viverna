#define SHININESS MATERIAL_PARAM0
#define DIFFUSE_TEXTURE TEXTURE0
#define SPECULAR_TEXTURE TEXTURE1

vec3 CalculatePointLights(in vec3 normal, in vec3 camera_pos);

void main() {
    vec3 normal = normalize(v.normal);
    vec3 cam_pos = vec3(camera.view_matrix[3]);

    vec3 result = CalculatePointLights(normal, cam_pos);

    f_color = vec4(result, 1.0);
}

float AttenuateLight(in PointLightData light, in float dist) {
    return 1.0
           / (light.ambient_constant.w + light.diffuse_linear.w * dist
              + light.specular_quadratic.w * dist * dist);
}

vec3 CalculatePointLights(in vec3 normal, in vec3 camera_pos) {
    vec3 result = vec3(0.0);
    vec3 to_camera = normalize(camera_pos - v.position);
    for (int i = 0; i < num_point_lights; i++) {
        PointLightData light = point_lights[i];
        vec3 to_light = light.position.xyz - v.position;
        float light_distance = length(to_light);
        to_light = (1.0 / light_distance) * to_light;
        vec3 halfway = normalize(to_light + to_camera);

        float diffuse_coeff = max(dot(normal, to_light), 0.0);
        float shine;
#ifdef VERNA_ANDROID
        // Hack: when SHININESS is 0.0, it gives weird results
        shine = max(SHININESS, 0.0001);
#else
        shine = SHININESS;
#endif
        float specular_coeff = pow(max(dot(normal, halfway), 0.0), shine);

        float attenuation = AttenuateLight(light, light_distance);

        vec3 diffuse_map = vec3(texture(DIFFUSE_TEXTURE, v.tex_coords));
        vec3 specular_map = vec3(texture(SPECULAR_TEXTURE, v.tex_coords));
        vec3 diffuse = light.diffuse_linear.xyz * diffuse_coeff * diffuse_map;
        vec3 specular =
            light.specular_quadratic.xyz * specular_coeff * specular_map;
        vec3 ambient = light.ambient_constant.xyz * diffuse_map;

        diffuse *= attenuation;
        specular *= attenuation;
        ambient *= attenuation;

        // TODO implement later
        float shadow = 0.0;
        result += ambient + (1.0 - shadow) * (diffuse + specular);
    }
    return result;
}