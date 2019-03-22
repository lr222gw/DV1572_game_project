#version 440 core

uniform sampler2D tex_diff;
uniform sampler2D tex_spec;
uniform sampler2D tex_norm;
uniform sampler2D tex_emit;
uniform sampler2D tex_disp;
uniform vec4 obj_id;

uniform vec4  rgba;
uniform float glow;

in  vec3 pos_fs;

out vec3 glPosition;

// @TAG{TEXTURE_CHANNEL}
layout (location = 0) out  vec4 g_albedo;
layout (location = 1) out  vec4 g_spec;
layout (location = 2) out  vec3 g_normal;
layout (location = 3) out  vec4 g_emit;
layout (location = 4) out  vec4 g_displacement;
layout (location = 5) out  vec3 g_position;
layout (location = 6) out  vec4 g_picking;

void main() {
    glPosition  = pos_fs;
   //g_position  = pos_fs;
   //g_albedo    = vec4( 0.0, 1.0, 0.0, 1.0 ); //rgba;
   //g_emit      = vec4( 1.0, 1.0, 0.4, 1.0 ); //glow * rgba;
}
