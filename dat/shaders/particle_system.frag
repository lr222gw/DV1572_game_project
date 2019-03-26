#version 440 core

in vec2  uv_fs;
in vec3 pos_fs;
in mat3 tbn_fs;
in vec4 col_fs;

uniform sampler2D tex_diff;
uniform sampler2D tex_spec;
uniform sampler2D tex_norm;
uniform sampler2D tex_emit;
uniform sampler2D tex_disp;

// @TAG{TEXTURE_CHANNEL}
layout (location = 0) out  vec4 g_albedo;
layout (location = 1) out  vec4 g_spec;
layout (location = 2) out  vec3 g_normal;
layout (location = 3) out  vec4 g_emit;
layout (location = 4) out  vec4 g_displacement;
layout (location = 5) out  vec3 g_position;

// @TAG{TEXTURE_CHANNEL}
void main() {
   g_albedo       = mix(texture(   tex_diff, uv_fs      ), col_fs, .5);// TODO: mix with col_fs (Dont know if this work, always mix by 50%?)
   g_spec         = texture(   tex_spec, uv_fs      );
   g_normal       = texture(   tex_norm, uv_fs      ).xyz;
   g_normal       = normalize( g_normal * 2.0 - 1.0 ); // tranform from [-1,1] to [0,1]
   g_normal       = normalize( tbn_fs * g_normal    ); // apply TBN matrix
   g_emit         = texture(   tex_emit, uv_fs      );
   g_displacement = texture(   tex_disp, uv_fs      );
   g_position     = pos_fs;

   // TODO: use glEnable(GL_BLEND) instead, first fix particle system to not store stuff in the RGBA's alpha channel
	if ( g_albedo.w < 0.525 ) discard; // hard-coded ugliness
}

// TODO: add texture atlasing to facilitate animated particles or multi-texture particle systems

// TODO: optimizing the fragment coverage by using a third party program to generate a non-quad mesh for the particle based off of the alpha values
