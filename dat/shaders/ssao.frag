#version 440 core

const uint  kernel_size       = 64; // needs to correspond to Kernel::ssao_kernel_count!
const float sampling_radius   = 0.500f;
const float depth_bias        = 0.025f;
const float noise_side        = 8;  // needs to correspond to Config::ssao_noise_side!
const vec2  noise_scale_factor = resolution / noise_tile_side; // needed to scale noise texture to screen size

in       vec2       uv_fs;
uniform  vec2       resolution;
uniform  vec3       kernel_samples[kernel_size];
uniform  mat4       projection_matrix;
uniform  sampler2D  g_pos_tex;
uniform  sampler2D  g_nor_tex;
uniform  sampler2D  noise_tex;
out      float      ssao_value;

void main() {
   vec3 fragment_pos = texture( g_pos_tex, uv_fs );
   vec3 fragment_nor = texture( g_nor_tex, uv_fs );
   vec3 fragment_rng = texture( noise_tex, noise_scale_factor * uv_fs );

   vec3 tangent      = normalize( fragment_rng - fragment_nor * dot(fragment_rng, fragment_nor) );
   vec3 bitanget     = cross(normal, tangent);
   mat3 tbn_matrix   = mat3( tangent, bitangent, normal );

   float occlusion   = 0.0f;
   for ( int i=0;  i < kernel_size;  ++i ) {
      vec3 sample        = tbn * kernel_samples[i]; // tangent to view space
      sample             = fragment_pos + sample * sampling_radius;
      vec4 offset        = projection_matrix * vec4( sample, 1.0f ); // view space to clip space
      offset.xyz         = offset.xyz / offset.w * 0.5 + 0.5;        // perspective divide & transform to normal range [0,1]
      float depth_sample = texture( g_pos_tex, offset.xy).z;
      float range_impact = smoothstep( 0.0f, 1.0f, sampling_radius / abs(fragment_pos.z + depth_sample) );
      if ( depth_sample >= sample.z + depth_bias )
         occlusion += 1.0f * range_impact;
   }

   ssao_value = 1.0 - occlusion / kernel_size;
}
