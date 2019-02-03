#version 440 core

out vec4 FragColor;
in  vec2 uv_fs;

uniform sampler2D tex_diff1;
uniform sampler2D tex_diff2;
uniform sampler2D tex_spec1;
uniform sampler2D tex_spec2;
uniform sampler2D tex_norm1;
uniform sampler2D tex_norm2;

void main() {    
    vec4 ambient = vec4(0.20, 0.20, 0.20, 1.0);

    vec4 a =texture2D(tex_diff1, uv_fs);
    vec4 b =texture2D(tex_diff2, uv_fs);
    vec4 result = mix(a,b ,0.5); 
    FragColor =      result  + ambient ;
                    //vec4(1.0f,1.0,0.0f,1.0f);
                    //texture( tex_diff1, uv_fs )
                    //+ texture( tex_spec1, uv_fs )
                    //+ texture( tex_norm1, uv_fs );
}
