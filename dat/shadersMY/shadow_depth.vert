#version 440 core

layout (location = 0) in vec3 pos_vs;

uniform mat4 lightmatrix;
uniform mat4 model_transform;

void main()
{
    gl_Position = lightmatrix * model_transform * vec4(pos_vs, 1.0);
}  