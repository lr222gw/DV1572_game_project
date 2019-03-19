#version 440 core

layout (location = 0) in vec3 pos_vs;

uniform mat4 lightmatrix;
uniform mat4 model_transform;

void main() {
   // transform vertex position from local space world space to light space (light source perspective instead of camera perspective)
   gl_Position = lightmatrix * model_transform * vec4(pos_vs, 1.0);
}
