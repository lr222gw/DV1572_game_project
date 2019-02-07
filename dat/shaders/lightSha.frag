#version 440 core

// This is just to test using their shader
// TODO: Make our own version of this shader

out vec4 rgba_rasterizer;

in vec2 uv_fs;

uniform sampler2D g_tex_pos;
uniform sampler2D g_tex_norm;
uniform sampler2D g_tex_spec;
uniform sampler2D g_tex_albedo;

struct Light {
  uint type;

  vec3 dir;
  vec3 pos;
  vec3 rgb;

  float intensity;
  float radius;
  float degree;
  float specularity;
};

float linear    = 0.7f;
float quadratic = 1.8f;

const int lights_cap = 32;

uniform Light lights[lights_cap];
uniform int num_lights;
uniform vec3 view_pos;

void main()
{             
    // retrieve data from gbuffer
    vec3 pos    = texture(g_tex_pos, uv_fs).rgb;
    vec3 norm   = texture(g_tex_norm, uv_fs).rgb;
    vec3 albedo = texture(g_tex_albedo, uv_fs).rgb;
	//float albedo_alpha = texture(g_tex_albedo, uv_fs).a;
    vec4 spec = texture(g_tex_spec, uv_fs).rgba;
    
    // then calculate lighting as usual
    vec3 lighting  = albedo + 0.2; // hard-coded ambient component
    vec3 view_dir  = normalize(view_pos - pos);
   for(int i = 0; i < num_lights; ++i)
   {
       // calculate distance between light source and current fragment
       //if(distance < light.radius)
       //{
       //}
	   Light light = lights[i];
	   float light_dist = length(light.pos - pos);
		// diffuse
		vec3 light_dir = normalize(light.pos - pos);
		vec3 diffuse = max(dot(norm, light.dir), 0.0) * albedo * light.rgb;
		// spec something
		vec3 halfway_dir = normalize(light_dir + view_dir);  

		//TODO: modify, make simple
		float spec_modulation = pow(max(dot(norm, halfway_dir), 0.0), 16.0);
		vec3 specular = mix(light.rgb, spec.rgb, 0.5) * spec_modulation * spec.a;
		// attenuation
		float attenuation = 1.0 / (1.0 + linear * light_dist + quadratic * light_dist * light_dist);
		diffuse *= attenuation;
		specular *= attenuation;
		lighting += diffuse + specular;
   }    

		
   rgba_rasterizer = vec4(lighting, 1.0);
}