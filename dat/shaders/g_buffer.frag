#version 440 core

in vec2 uv_fs;
in vec3 pos_fs;
in vec3 nor_fs;
in mat3 tbn_fs;

uniform sampler2D tex_diff;
uniform sampler2D tex_spec;
uniform sampler2D tex_norm;
uniform sampler2D tex_emit;

uniform uvec4 obj_id;

// @TAG{TEXTURE_CHANNEL}
layout (location = 0) out  vec4 g_albedo;
layout (location = 1) out  vec4 g_spec;
layout (location = 2) out  vec3 g_normal;
layout (location = 3) out  vec4 g_emit;
layout (location = 4) out  vec3 g_position;
layout (location = 5) out  vec4 g_picking;

void main() {
	g_position = pos_fs;
	g_normal   = texture( tex_norm, uv_fs ).xyz;
	//TODO: Fix wierd Model bugg, half has a different color...
	g_normal   = normalize(g_normal * 2.0 -1.0);  // G�r om s� vi f�r mellan -1 och 1 ist�llet f�r 0 och 1. Beh�vs kanske inte f�r se
	g_normal   = normalize(tbn_fs * g_normal);
	g_spec     = texture( tex_spec, uv_fs );
	g_albedo   = texture( tex_diff, uv_fs );
	g_emit     = texture( tex_emit, uv_fs );
	//g_spec	  = obj_id; // här ifall jag vill se picking texturen
	g_picking  = obj_id;
}
