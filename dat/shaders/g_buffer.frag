#version 440 core

in vec2 uv_fs;
in vec3 pos_fs;
in vec3 nor_fs;


uniform sampler2D tex_diff1;
uniform sampler2D tex_diff2;
uniform sampler2D tex_spec1;
uniform sampler2D tex_spec2;
uniform sampler2D tex_norm1;
uniform sampler2D tex_norm2;

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec4 g_normal;
layout (location = 2) out vec4 g_spec;
layout (location = 3) out vec4 g_albedo;

void main() {
	g_position = pos_fs;
	g_normal   = texture( tex_norm1, uv_fs );
	g_normal   = normalize(g_normal * 2.0 -1.0);  // G�r om s� vi f�r mellan -1 och 1 ist�llet f�r 0 och 1. Beh�vs kanske inte f�r se 
	g_spec     = texture( tex_spec1, uv_fs );
	g_albedo   = texture( tex_diff1, uv_fs );
}












// normalize( nor_fs ); // g_normal = texture(tex_norm1, uv_fs).rgb;
