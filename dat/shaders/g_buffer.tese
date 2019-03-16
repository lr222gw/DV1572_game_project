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


void main()
{

	vec2 uv1 = mix(uv_te[1],uv_te[0],gl_TessCoord.x);
	vec2 uv2 = mix(uv_te[2],uv_te[3],gl_TessCoord.x);
	uv_gs = mix(uv1, uv2, gl_TessCoord.y);

	//TODO: Check if "tbn_te[1][0]" is the Tangent Vector...
	vec3 tangent1 = mix(tbn_te[1][0],tbn_te[0][0],gl_TessCoord.x);
	vec3 tangent2 = mix(tbn_te[2][0],tbn_te[3][0],gl_TessCoord.x);
	vec3 tangent = mix(tangent1, tangent2, gl_TessCoord.y);

	vec3 bitangent1 = mix(tbn_te[1][1],tbn_te[0][1],gl_TessCoord.x);
	vec3 bitangent2 = mix(tbn_te[2][1],tbn_te[3][1],gl_TessCoord.x);
	vec3 bitangent = mix(bitangent1, bitangent2, gl_TessCoord.y);

	vec3 normal1 = mix(tbn_te[1][2],tbn_te[0][2],gl_TessCoord.x);
	vec3 normal2 = mix(tbn_te[2][2],tbn_te[3][2],gl_TessCoord.x);
	vec3 normal = mix(normal1, normal2, gl_TessCoord.y);

	tbn_gs = mat3(tangent, bitangent, normal);

	//TODO: Conversion:  to vec4 or gl_TessCoord to vec3?
	vec4 p1 = mix(vec4(pos_te[1],1.0f),vec4(pos_te[0],1.0f),gl_TessCoord.x);
	vec4 p2 = mix(vec4(pos_te[2],1.0f),vec4(pos_te[3],1.0f),gl_TessCoord.x);
	vec4 pos = mix(p1, p2, gl_TessCoord.y);

	float displacement = texture(displacement_map, uv_gs.xy).x;
	// Displace the vertex along the normal
	pos_gs = (pos.xyz + normal * displacement * 23); //TODO: add Factor instead of hardcoded "23"

	gl_Position = projection * view * vec4(pos_gs,1.0f);


} 