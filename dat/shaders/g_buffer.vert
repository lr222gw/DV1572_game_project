#version 440 core

layout (location = 0) in vec3 pos_vs;
layout (location = 1) in vec3 nor_vs;
layout (location = 2) in vec3 tan_vs;
layout (location = 3) in vec3 bit_vs;
layout (location = 4) in vec2  uv_vs;

uniform mat4 model_transform;
uniform mat4 view;
uniform mat4 projection;

out vec3 pos_gs;
out vec2 uv_gs;
out mat3 tbn_gs;

void main() {
	uv_gs  = uv_vs;

	vec3 tang = normalize( vec3(model_transform * vec4(tan_vs, 0.0) ) ); // provided by assimp; transforming to world-space
	vec3 btan = normalize( vec3(model_transform * vec4(bit_vs, 0.0) ) ); // provided by assimp; transforming to world-space
	vec3 norm = normalize( vec3(model_transform * vec4(nor_vs, 0.0) ) ); // provided by assimp; transforming to world-space
	tbn_gs    = mat3(tang, btan, norm);

	vec4 pos_world = model_transform * vec4( pos_vs, 1.0 ); // vertex pos to world-space
	pos_gs         = pos_world.xyz;

	gl_Position    = projection * view  * pos_world; // vertex pos to screen-space
}
