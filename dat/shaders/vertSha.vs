#version 440 core

layout (location = 0) in vec3 pos_vs;
layout (location = 1) in vec3 normal_vs;
layout (location = 2) in vec2 uv_vs;

out vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    uv_fs = uv_vs;    
    gl_Position = projection * view * model * vec4(pos_vs, 1.0);
}