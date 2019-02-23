
billboard physics:


geometry shader:

uint                 max_capacity
vec3[max_capacity]   positions
float[max_capacity]  scales
uint                 particle_count




fragment shader:

uint                max_capacity
vec3[max_capacity]  positions
uint                particle_count

uniform sampler2D tex_diff;
uniform sampler2D tex_spec;
uniform sampler2D tex_norm;
uniform sampler2D tex_emit;
