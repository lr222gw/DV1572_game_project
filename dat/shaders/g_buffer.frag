#version 440 core

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_spec;
layout (location = 3) out vec4 g_albedo;

in vec3 pos_fs;
in vec3 norm_fs;
in vec2 uv_fs;

uniform sampler2D tex_spec1;
uniform sampler2D tex_diff1;

void main()
{
	
	g_position = pos_fs;
	g_normal = normalize(norm_fs);
	g_spec = texture(tex_diff1, uv_fs);
	g_albedo = texture(tex_spec1, uv_fs);



}