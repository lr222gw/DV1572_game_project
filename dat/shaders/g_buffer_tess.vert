#version 440 core

layout (location = 0) in vec3 pos_vs;
layout (location = 1) in vec3 nor_vs;
layout (location = 2) in vec3 tan_vs;
layout (location = 3) in vec3 bit_vs;
layout (location = 4) in vec2 uv_vs;

uniform mat4 model_transform;

out vec3 pos_tc;
out vec2 uv_tc;
out mat3 tbn_tc;

void main() {
	uv_tc  = uv_vs;

	vec3 tang = normalize( vec3(model_transform * vec4(tan_vs, 0.0) ) );
	vec3 btan = normalize( vec3(model_transform * vec4(bit_vs, 0.0) ) );
	vec3 norm = normalize( vec3(model_transform * vec4(nor_vs, 0.0) ) );

	tbn_tc = mat3(tang, btan, norm);

	vec4 pos_world = model_transform * vec4( pos_vs, 1.0 );
	pos_tc         = pos_world.xyz;

	//Output for vertex to Tessepipeline should not be in ClipSpace...
	gl_Position    = pos_world;
	//gl_Position    = projection * view  * pos_world;
}
