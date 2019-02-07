#version 440 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gSpec;
layout (location = 3) out vec4 gAlbedo;


out vec4 FragColor;
in vec2 uv_fs;
in vec3 pos_fs;
in vec3 nor_fs;

uniform sampler2D tex_diff1;
uniform sampler2D tex_diff2;
uniform sampler2D tex_spec1;
uniform sampler2D tex_spec2;
uniform sampler2D tex_norm1;
uniform sampler2D tex_norm2;

void main() {    
    vec4 ambient = vec4(0.20, 0.20, 0.20, 1.0);

	gPosition = pos_fs;
	
	//TODO: Use Normal texture instead of Calculated Normal 
	gNormal = normalize(nor_fs);
	
	gAlbedo.rgba = texture(tex_diff1, uv_fs).rgba;
	gSpec.rgba = texture(tex_spec1, uv_fs).rgba;
    //vec4 a =texture2D(tex_diff1, uv_fs);
    //vec4 b =texture2D(tex_diff2, uv_fs);
    //vec4 result = mix(a,b ,0.5); 
    //FragColor =      a  + ambient ;
                    //vec4(1.0f,1.0,0.0f,1.0f);
                    //texture( tex_diff1, uv_fs )
                    //+ texture( tex_spec1, uv_fs )
                    //+ texture( tex_norm1, uv_fs );
}
