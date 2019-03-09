#version 440 core

in vec2  uv_fs;
in vec3 pos_fs;
in vec3 nor_fs;
in mat3 tbn_fs;

uniform sampler2D tex_diff;
uniform sampler2D tex_spec;
uniform sampler2D tex_norm;
uniform sampler2D tex_emit;

// @TAG{TEXTURE_CHANNEL}
layout (location = 0) out vec4 g_albedo;
layout (location = 1) out vec4 g_spec;
layout (location = 2) out vec3 g_normal;
layout (location = 3) out vec4 g_emit;
layout (location = 4) out vec3 g_position;

void main() {
   g_position = pos_fs;
   g_normal   = texture(   tex_norm, uv_fs ).xyz;
   g_normal   = normalize( g_normal * 2.0 - 1.0 ); // TODO: verify necessity
   g_normal   = normalize( tbn_fs * g_normal );
   g_spec     = texture(   tex_spec, uv_fs );
   g_albedo   = texture(   tex_diff, uv_fs );
   g_emit     = texture(   tex_emit, uv_fs );
}
