#version 440 core

layout(quads, equal_spacing, cw) in; //TODO: Clockwise or Counter Clockwise?

uniform mat4 view;
uniform mat4 projection;
uniform sampler2D displacement_map;
//uniform float displacementFactor;
 
in vec3 pos_te[];
in vec2 uv_te[];
in mat3 tbn_te[];

out vec3 pos_gs;
out vec2 uv_gs ;
out mat3 tbn_gs;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
} 

void main()
{



	//vec4 p1 = mix(gl_in[1].gl_Position,gl_in[0].gl_Position,gl_TessCoord.x);
	//vec4 p2 = mix(gl_in[2].gl_Position,gl_in[3].gl_Position,gl_TessCoord.x);
	
	// Displace the vertex along the normal
	WorldPos_FS_in += Normal_FS_in * Displacement * gDispFactor;

	gl_Position = mix(p1, p2, gl_TessCoord.y);



    float Displacement = texture(gDisplacementMap, TexCoord_FS_in.xy).x;
    WorldPos_FS_in += Normal_FS_in * Displacement * gDispFactor;
    gl_Position = gVP * vec4(WorldPos_FS_in, 1.0);



	pos_gs = pos_te[gl_InvocationID];
	uv_gs = uv_te[gl_InvocationID];
	tbn_gs = tbn_te[gl_InvocationID]; 
} 