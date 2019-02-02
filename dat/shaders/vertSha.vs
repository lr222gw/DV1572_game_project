#version 440 core

layout (location = 0) in vec3 pos_vs;
layout (location = 1) in vec3 normal_vs;
layout (location = 2) in vec2 uv_vs;

out vec2 uv_fs;

uniform mat4 mvp_transform;
uniform mat4 model_transform;

void main()
{
    uv_fs = uv_vs;    	
    gl_Position = view_tranform * model_transform * vec4(pos_vs, 1.0);
}