#version 440
layout ( triangles    ) in;
layout ( triangle_strip, max_vertices = 3) out;

in vec2 uv_gs[];
in vec3 pos_gs[];
in vec3 nor_gs[];


out vec2 uv_fs;
out vec3 pos_fs;
out vec3 nor_fs;


void main(void){

    gl_Position = gl_in[0].gl_Position;
    //gs_color = vs_color[0];
	uv_fs = uv_gs[0];
	pos_fs = pos_gs[0];
	nor_fs = nor_gs[0];
    EmitVertex();
    
    gl_Position = gl_in[1].gl_Position;
    //gs_color = vs_color[1];
	uv_fs = uv_gs[1];
	pos_fs = pos_gs[1];
	nor_fs = nor_gs[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    //gs_color = vs_color[2];
	uv_fs = uv_gs[2];
	pos_fs = pos_gs[2];
	nor_fs = nor_gs[2];
    EmitVertex();

    EndPrimitive();    
}