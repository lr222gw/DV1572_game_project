#version 440 core
// vs filer finnes
layout (location = 0) in vec3 pos_vs;
layout (location = 1) in vec2 uv_vs;

out vec2 uv_fs;

void main()
{
    gl_Position = vec4(pos_vs, 1.0);
    uv_fs = uv_vs;
}