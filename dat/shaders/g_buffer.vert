#version 440 core

layout (location = 0) in vec3 pos_vs;
layout (location = 1) in vec3 nor_vs;
layout (location = 2) in vec2 uv_vs;

uniform mat4 model_transform;
uniform mat4 view;
uniform mat4 projection;

out vec3 pos_fs;
out vec3 nor_fs;
out vec2 uv_fs;

void main() {
	uv_fs = uv_vs;

	mat3 norm_mat3 = transpose( inverse( mat3(model_transform) ) );
	nor_fs         = norm_mat3 * nor_vs;

	vec4 pos_world = model_transform * vec4( pos_vs, 1.0 );
	pos_fs         = pos_world.xyz;

	gl_Position    = projection * view * pos_world;
}
