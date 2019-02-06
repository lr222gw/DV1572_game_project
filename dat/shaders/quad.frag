#version 440 core
// TODO: make our own version of this
layout (location = 0) out vec4 FragColor;

uniform vec3 lightColor;

void main()
{           
    FragColor = vec4(lightColor, 1.0);
}