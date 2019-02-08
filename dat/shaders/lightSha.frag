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

//float linear    = 0.7f;
//float quadratic = 1.8f;

const int lights_cap = 32;

const uint light_type_point       = 0,
           light_type_spot        = 1,
           light_type_directional = 2;


uniform Light lights[lights_cap];

// TODO: separate arrays for separate types

uniform int  num_lights;
uniform vec3 view_pos;

void main() {             
   // retrieve data from gbuffer
   vec3  pos      = texture(g_tex_pos,    uv_fs).rgb;
   vec3  norm     = texture(g_tex_norm,   uv_fs).rgb;
   vec3  albedo   = texture(g_tex_albedo, uv_fs).rgb;
   vec3  spec_rgb = texture(g_tex_spec,   uv_fs).rgb;
   float spec_str = texture(g_tex_spec,   uv_fs).w;

   vec3 view_dir  = normalize( view_pos - pos );

   vec3 lighting  = albedo * 0.2; // hard-coded ambient component

   for ( int i = 0;  i  < 1;  ++i ) {
      Light light = lights[i];
      if ( light.type == light_type_point ) {
      ////////////////////////////////////////////////////////////////////////////////////////
         float radius      = light.radius * 20.0f;
         float distance    = length( light.pos - pos );
         if ( distance < radius ) {
            // calculate light impact:
            vec3  light_dir        = normalize( light.pos - pos );
            float linear_falloff   = (1.0 - distance / radius); // * light.intensity;
            float quad_falloff     = linear_falloff * linear_falloff;
            float light_modulation = dot( norm, light_dir ) / 2.0 + 0.5;
            lighting              += light.rgb * (light_modulation * quad_falloff);
            // calculate specular:

         }
         /*
         vec3  light_dir   = normalize( light.pos - pos );
         float light_dist  = length(    light.pos - pos ) / radius;
         vec3  halfway_dir = normalize( light_dir + view_dir );
         vec3  diffuse     = max( dot(norm, light.dir), 0.0 )
                             * light.rgb
                             * light.intensity
                             * albedo;

         if ( radius > light_dist) {
            float spec_modulation  = pow( max( dot(norm, halfway_dir), 0.0 ), 16.0);
            float light_modulation = max( dot(norm, light_dir), 0.0 );
            vec3  specular         = light.rgb * spec_modulation  * spec_str;
            vec3  phong            = light.rgb * light_modulation * light.intensity;
            float attenuation      = 1.0f;
            //float attenuation      = light.rgb * light_modulation * light.intensity;
            diffuse  *= attenuation; // * light.intensity;
            specular *= attenuation; // * light.specularity;
            phong    *= attenuation;
            lighting += diffuse + specular + phong;
         }
         */
      ////////////////////////////////////////////////////////////////////////////////////////
      }
      else if ( light.type == light_type_spot ) {
         lighting = vec3(1.0, 0.0, 1.0 ); // TODO
      }
      else if ( light.type == light_type_directional ) {
         lighting = vec3(1.0, 0.0, 1.0 ); // TODO
      }
      else {
         lighting = vec3(1.0, 0.0, 1.0 ); // TODO
      }
   }

   rgba_rasterizer = vec4(lighting, 1.0);
}