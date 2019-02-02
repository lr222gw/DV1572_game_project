#version 440 core

out vec4 FragColor;
in  vec2 uv_fs;

uniform sampler2D tex_diff1;
uniform sampler2D tex_spec1;
uniform sampler2D tex_norm1;

void main() {    
    FragColor = texture( tex_diff1, uv_fs )
              + texture( tex_spec1, uv_fs )
              + texture( tex_norm1, uv_fs );
}
