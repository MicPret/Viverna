#define SHININESS MATERIAL_PARAM0
#define DIFFUSE_TEXTURE TEXTURE0
#define SPECULAR_TEXTURE TEXTURE1

in vec3 v_position;

vec3 CalculatePointLights(in vec3 normal, in vec3 camera_pos);

void main() {
    vec3 normal = normalize(v_normal);
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
    vec3 to_camera = normalize(camera_pos - v_position);
    for (int i = 0; i < num_point_lights; i++) {
        PointLightData light = point_lights[i];
        vec3 to_light = light.position - v_position;
        float light_distance = length(to_light);
        to_light = (1.0 / light_distance) * to_light;
        vec3 halfway = normalize(to_light + to_camera);

        float diffuse_coeff = max(dot(normal, to_light), 0.0);
        float specular_coeff = pow(max(dot(normal, halfway), 0.0), SHININESS);

        float attenuation = AttenuateLight(light, light_distance);

        vec3 diffuse_map = vec3(texture(DIFFUSE_TEXTURE, v_tex_coords));
        vec3 specular_map = vec3(texture(SPECULAR_TEXTURE, v_tex_coords));
        vec3 diffuse = light.diffuse_linear.xyz * diffuse_coeff * diffuse_map;
        vec3 specular =
            light.specular_quadratic.xyz * specular_coeff * specular_map;
        vec3 ambient = light.ambient_constant.xyz * diffuse_map;

        diffuse *= light_attenuation;
        specular *= light_attenuation;
        ambient *= light_attenuation;

        float shadow = 0.0;  // TODO implement later
        result += ambient + (1.0 - shadow) * (diffuse + specular);
    }
    return result;
}