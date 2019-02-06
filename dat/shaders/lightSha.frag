#version 440 core

// This is just to test using their shader
// TODO: Make our own version of this shader

out vec4 FragColor;

in vec2 uv_fs;

uniform sampler2D g_tex_pos;
uniform sampler2D g_tex_norm;
uniform sampler2D g_tex_spec;
uniform sampler2D g_tex_albedo;


struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float Radius;
};
//const int NR_LIGHTS = 32;
//uniform Light lights[NR_LIGHTS];
vec3 viewPos = vec3(0.0f, 5.0f, -10.0f);

vec3 light_pos = vec3(3.0f, 5.0f, 0.0f);
vec3 light_col = vec3(0.3f, 0.7, 0.1);
float Linear = 0.7f;
float Quadratic = 1.8f;
float Radius = 5;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(g_tex_pos, uv_fs).rgb;
    vec3 Normal = texture(g_tex_norm, uv_fs).rgb;
    vec3 albedo = texture(g_tex_albedo, uv_fs).rgb;
    float spec = texture(g_tex_spec, uv_fs).a;
    
    // then calculate lighting as usual
    vec3 lighting  = albedo * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
//    for(int i = 0; i < NR_LIGHTS; ++i)
//    {
//        // calculate distance between light source and current fragment
//        if(distance < lights[i].Radius)
//        {
//        }
//    }    
   float distance = length(light_pos - FragPos);
   // diffuse
   vec3 lightDir = normalize(light_pos - FragPos);
   vec3 diffuse = max(dot(Normal, lightDir), 0.0) * albedo * light_col;
   // specular
   vec3 halfwayDir = normalize(lightDir + viewDir);  
   float spec1 = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
   vec3 specular = light_col * spec1 * spec;
   // attenuation
   float attenuation = 1.0 / (1.0 + Linear * distance + Quadratic * distance * distance);
   diffuse *= attenuation;
   specular *= attenuation;
   lighting += diffuse + specular;
   FragColor = vec4(lighting, 1.0);
}