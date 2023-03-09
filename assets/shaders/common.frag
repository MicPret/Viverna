#ifdef VERNA_DESKTOP
#line 3
#define DRAW_ID int(v.MESH_IDX)
#endif
#ifdef VERNA_ANDROID
uniform int DRAW_ID;
#endif

in VS_OUT {
    float MESH_IDX;
    vec3 position;
    vec2 tex_coords;
    vec3 normal;
}
v;

out vec4 f_color;

// Blinn-Phong

#define SHININESS MATERIAL_PARAM0
#define DIFFUSE_TEXTURE TEXTURE0
#define SPECULAR_TEXTURE TEXTURE1

float LightAttenuation(in PointLightData light, in float dist) {
    return 1.0
           / (light.ambient_constant.w + light.diffuse_linear.w * dist
              + light.specular_quadratic.w * dist * dist);
}

vec3 BlinnPhongAmbient(in PointLightData light) {
    return light.ambient_constant.xyz;
}

vec3 BlinnPhongDiffuse(in PointLightData light,
                       in vec3 diffuse_color,
                       in vec3 normal,
                       in vec3 to_light) {
    float coeff = max(dot(normal, to_light), 0.0);
    return coeff * light.diffuse_linear.xyz * diffuse_color;
}

vec3 BlinnPhongSpecular(in PointLightData light,
                        in vec3 specular_color,
                        in vec3 normal,
                        in vec3 halfway,
                        in float shine) {
    float coeff = pow(max(dot(normal, halfway), 0.0), shine);
    return coeff * light.specular_quadratic.xyz * specular_color;
}
