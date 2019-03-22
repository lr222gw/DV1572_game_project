#version 440 core

uniform mat4 model_transform;
uniform mat4 view;
uniform mat4 projection;

layout  (location = 0) in vec3 pos_in;

out vec3 pos_fs;

void main() {
   vec4 pos_world = model_transform * vec4( pos_in, 1.0 ); // vertex pos to world-space
   pos_fs         = pos_world.xyz;
   gl_Position    = projection * view * pos_world; // vertex pos to screen-space
}
