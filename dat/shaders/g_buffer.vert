#version 440 core

layout (location = 0) in vec3 pos_vs;
layout (location = 1) in vec3 nor_vs;
layout (location = 2) in vec3 tan_vs;
layout (location = 3) in vec3 bit_vs;
layout (location = 4) in vec2 uv_vs;

uniform mat4 model_transform;
uniform mat4 view;
uniform mat4 projection;

out vec3 pos_gs;
// out vec3 nor_gs;
out vec2 uv_gs;
out mat3 tbn_gs;

void main() {
	uv_gs  = uv_vs;

	vec3 tang = normalize( vec3(model_transform * vec4(tan_vs, 0.0) ) );
	vec3 btan = normalize( vec3(model_transform * vec4(bit_vs, 0.0) ) );
	vec3 norm = normalize( vec3(model_transform * vec4(nor_vs, 0.0) ) );
	//mat3 tbn = mat3(tang, btan, norm);
	if ( dot( cross(norm, tang), btan ) < 0.0 ) {
		tang = tang * -1.0;
	}
	tbn_gs = mat3(tang, btan, norm);

//	mat3 norm_mat3 = transpose( inverse( mat3(model_transform) ) );
//	nor_gs         = norm_mat3 * nor_vs;

	vec4 pos_world = model_transform * vec4( pos_vs, 1.0 );
	pos_gs         = pos_world.xyz;

	gl_Position    = projection * view  * pos_world;
}
