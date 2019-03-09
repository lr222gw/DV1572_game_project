#version 440 core

layout ( triangles    ) in;
layout ( triangle_strip, max_vertices = 3) out;

in vec2 uv_gs[];
in vec3 pos_gs[];
in mat3 tbn_gs[];


out vec2 uv_fs;
out vec3 pos_fs;
out mat3 tbn_fs;

uniform vec3 view_pos;
uniform mat4 view;
uniform mat4 model_transform;
uniform mat4 projection;

void main(void) {

	//float sum1 = gl_in[1].gl_Position.x - gl_in[0].gl_Position.x;
	//float sum2 = gl_in[1].gl_Position.y + gl_in[0].gl_Position.y;
	//
	//float sum_1 = sum1*sum2;
	//
	//float sum3 = gl_in[2].gl_Position.x - gl_in[1].gl_Position.x;
	//float sum4 = gl_in[2].gl_Position.y + gl_in[1].gl_Position.y;
	//
	//float sum_2 = sum3*sum4;
	//
	//float sum5 = gl_in[0].gl_Position.x - gl_in[2].gl_Position.x;
	//float sum6 = gl_in[0].gl_Position.y + gl_in[2].gl_Position.y;
	//
	//float sum_3 = sum5*sum6;
	//
	//float sumTotal = sum_1+sum_2+sum_3;
	//if(sumTotal < 0.001 ){

	//Possible solution
	//vec3 viewPos = view_pos;
	//viewPos.z =  view_pos.z -1000.0f; // if "-1000" is smaller, More artifacts...
	//vec3 vector_a =   normalize(viewPos.xyz) - (gl_in[0].gl_Position.xyz);
	////vec3 normal = cross((gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz),(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz));
	//vec3 normal = cross( (gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz), (gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz));
	//float result = max(dot(normalize(normal), vector_a ), 0);

	//vec3 vect1 = (gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz);
	//vec3 vect2 = (gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);

	//float area = (1/2) * length(cross( vect2, vect1));

	vec4 viewPos   = projection * view * vec4(view_pos,1) ;
	viewPos.z      = view_pos.z - 1500.0f; // if "-1000" is smaller, More artifacts...
	vec3  vector_a = normalize(viewPos.xyz) - (gl_in[0].gl_Position.xyz);
	vec3  normal   = cross( (gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz), (gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz));
	float result   = max( dot( normalize(normal), vector_a ), 0 );


	if(result > 0.0 ){
	    gl_Position = gl_in[0].gl_Position;
		//gs_color = vs_color[0];
		uv_fs  = uv_gs[0];
		pos_fs = pos_gs[0];
		tbn_fs = tbn_gs[0];
		EmitVertex();

		gl_Position = gl_in[1].gl_Position;
		//gs_color = vs_color[1];
		uv_fs  = uv_gs[1];
		pos_fs = pos_gs[1];
		tbn_fs = tbn_gs[1];
		EmitVertex();

		gl_Position = gl_in[2].gl_Position;
		//gs_color = vs_color[2];
		uv_fs  = uv_gs[2];
		pos_fs = pos_gs[2];
		tbn_fs = tbn_gs[2];
		EmitVertex();

		EndPrimitive();
	}

}
