#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal_vs;
layout (location = 2) in vec2 uv_vs;


uniform mat4 model_transform; 
uniform mat4 view;
uniform mat4 projection;

void main() {

	gl_Position = projection * view * model_transform * vec4( position, 1.0f );
}