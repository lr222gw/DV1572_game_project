#version 440 core
layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3) out;

in vec2  uv_gs[];
in vec3 pos_gs[];
in mat3 tbn_gs[];

out vec2  uv_fs;
out vec3 pos_fs;
out mat3 tbn_fs;

uniform vec3 view_pos;
uniform mat4 view;
uniform mat4 model_transform;
uniform mat4 projection;

void main(void) {

	//set up for backface culling by using dotproduct between normal and vector from camera to current vertex
	vec4 viewPos   = projection * view * vec4(view_pos,1) ;
	viewPos.z      = view_pos.z - 1500.0f; // if "-1000" is smaller, More artifacts...
	vec3  vector_a = normalize(viewPos.xyz) - (gl_in[0].gl_Position.xyz);
	vec3  normal   = cross( (gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz), (gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz));
	float result   = max( dot( normalize(normal), vector_a ), 0 );


	//TODO: Interpolera vertricerna here?
	if(result > 0.0 ){ //TODO: REMOVE AFTER DEMO

	    gl_Position = gl_in[0].gl_Position;
		uv_fs  = uv_gs[0];
		pos_fs = pos_gs[0];
		tbn_fs = tbn_gs[0];
		EmitVertex();

		gl_Position = gl_in[1].gl_Position;
		uv_fs  = uv_gs[1];
		pos_fs = pos_gs[1];
		tbn_fs = tbn_gs[1];
		EmitVertex();

		gl_Position = gl_in[2].gl_Position;
		uv_fs  = uv_gs[2];
		pos_fs = pos_gs[2];
		tbn_fs = tbn_gs[2];
		EmitVertex();

		EndPrimitive();
	}

}
