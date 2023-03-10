void main() {
    vec3 normal = normalize(v.normal);
    vec3 cam_pos = vec3(camera.view_matrix[3]);
    vec3 to_camera = normalize(cam_pos - v.position);

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
    for (int i = 0; i < num_point_lights; i++) {
        PointLightData light = point_lights[i];
        vec3 to_light = light.position.xyz - v.position;
        float light_distance = length(to_light);
        to_light = to_light / light_distance;
        vec3 halfway = normalize(to_light + to_camera);

        vec3 ambient = BlinnPhongAmbient(light);
        vec3 diffuse = BlinnPhongDiffuse(light, diffuse_map, normal, to_light);
        vec3 specular =
            BlinnPhongSpecular(light, specular_map, normal, halfway, shine);
        float attenuation = LightAttenuation(light, light_distance);
        // TODO implement!
        float shadow = 0.0;
        result +=
            attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
    }

    f_color = vec4(result, 1.0);
}