#version 440 core

in vec2  uv_fs;
in vec3 pos_fs;
in mat3 tbn_fs;

uniform sampler2D tex_diff;
uniform sampler2D tex_spec;
uniform sampler2D tex_norm;
uniform sampler2D tex_emit;
uniform sampler2D tex_disp;

uniform vec4 obj_id;

// @TAG{TEXTURE_CHANNEL}
layout (location = 0) out  vec4 g_albedo;
layout (location = 1) out  vec4 g_spec;
layout (location = 2) out  vec3 g_normal;
layout (location = 3) out  vec4 g_emit;
layout (location = 4) out  vec4 g_displacement;
layout (location = 5) out  vec3 g_position;
layout (location = 6) out  vec4 g_picking;

void main() {
   g_albedo       = texture(   tex_diff, uv_fs     );
   g_spec         = texture(   tex_spec, uv_fs     );
	g_normal       = texture(   tex_norm, uv_fs     ).xyz;
	g_normal       = normalize( g_normal * 2.0 -1.0 ); // transform from [0,1] range to [-1,1] range
	g_normal       = normalize( tbn_fs   * g_normal );
	g_emit         = texture(   tex_emit, uv_fs     );
   g_displacement = texture(   tex_disp, uv_fs     );
   g_position     = pos_fs;
	g_picking      = obj_id;
}
