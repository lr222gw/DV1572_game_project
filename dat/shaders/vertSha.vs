#version 440 core

layout (location = 0) in vec3 pos_vs;
layout (location = 1) in vec3 normal_vs;
layout (location = 2) in vec2 uv_vs;

out vec2 uv_fs;

uniform mat4 viewport_transform;   // för Viewport

uniform mat4 model_transform; // för ModelInstances;
// ModelInstance har en _model av typ SharedPtr<Model>
// alla vertriser i _model är i local space (origo)
// model_transform konverterar alla koordinaterna från
// local space till global space relativt instansen.

void main()
{
    uv_fs = uv_vs;    	
    gl_Position = viewport_transform * model_transform * vec4(pos_vs, 1.0);
}