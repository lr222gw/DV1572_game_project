#version 440 core

layout (location = 0) in  vec3 pos_vs;
layout (location = 1) in  vec2 uv_vs;
//layout (location = 2) in  vec3 normal;
         //             out vec2 uv_fs;

out VS_OUT {
    vec3 FragPos;
    //vec3 Normal;
    vec2 uv;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 model_transform;
uniform mat4 lightmatrix;

void main() {
	vs_out.FragPos = vec3(model_transform * vec4(pos_vs, 1.0)).xyz; //vec3(model_transform * vec4(pos_vs, 1.0));
	//vs_out.Normal = transpose(inverse(mat3(model_transform))) * normal;
	vs_out.uv = uv_vs;
	vs_out.FragPosLightSpace = lightmatrix * vec4(vs_out.FragPos, 1.0);

	//uv_fs       = uv_vs;
	gl_Position = vec4(pos_vs, 1.0);
}
