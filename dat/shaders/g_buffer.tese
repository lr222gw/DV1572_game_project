#version 440 core

layout(triangles, equal_spacing, ccw) in; //TODO: Clockwise or Counter Clockwise?

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

	//vec2 uv1 = mix(uv_te[1],uv_te[3],gl_TessCoord.x);
	//vec2 uv2 = mix(uv_te[2],uv_te[0],gl_TessCoord.x);
	//uv_gs = mix(uv1, uv2, gl_TessCoord.y);
	//
	////TODO: Check if "tbn_te[1][0]" is the Tangent Vector...
	//vec3 tangent1 = mix(tbn_te[1][0],tbn_te[0][0],gl_TessCoord.x);
	//vec3 tangent2 = mix(tbn_te[2][0],tbn_te[3][0],gl_TessCoord.x);
	//vec3 tangent = mix(tangent1, tangent2, gl_TessCoord.y);
	//
	//vec3 bitangent1 = mix(tbn_te[1][1],tbn_te[0][1],gl_TessCoord.x);
	//vec3 bitangent2 = mix(tbn_te[2][1],tbn_te[3][1],gl_TessCoord.x);
	//vec3 bitangent = mix(bitangent1, bitangent2, gl_TessCoord.y);
	//
	//vec3 normal1 = mix(tbn_te[1][2],tbn_te[0][2],gl_TessCoord.x);
	//vec3 normal2 = mix(tbn_te[2][2],tbn_te[3][2],gl_TessCoord.x);
	//vec3 normal = mix(normal1, normal2, gl_TessCoord.y);
	//
	//tbn_gs = mat3(tangent, bitangent, normal);
	//
	////TODO: Conversion:  to vec4 or gl_TessCoord to vec3?
	//vec3 p1 = mix(pos_te[1],pos_te[0],gl_TessCoord.x);
	//vec3 p2 = mix(pos_te[2],pos_te[3],gl_TessCoord.x);
	//vec3 pos = mix(p1, p2, gl_TessCoord.y);
	////vec4 pos = vec4(0.0f);


	//const vec4 bc = vec4(1,3,3,1);
	//for(int j = 0; j < 4; ++j){
	//	for(int i = 0; i < 4; ++i){
	//		pos += bc[i] * pow( gl_TessCoord.x, i) * pow(1.0 - gl_TessCoord.x, 3-i) * bc[j] * pow( gl_TessCoord.y, j) * pow(1.0 - gl_TessCoord.y, 3-j ) * vec4(pos_te[4*j+i],1.0f);
	//	}
	//}

	//float displacement = texture(displacement_map, uv_gs.xy).x;
	// Displace the vertex along the normal
	//pos_gs = (pos.xyz + normal * displacement * 23); //TODO: add Factor instead of hardcoded "23"


	vec3 tangent = interpolate3D(tbn_te[0][0],tbn_te[1][0],tbn_te[2][0]);
	
	vec3 bitangent = interpolate3D(tbn_te[0][1],tbn_te[1][1],tbn_te[2][1]);
	
	vec3 normal = interpolate3D(tbn_te[0][2],tbn_te[1][2],tbn_te[2][2]);
	
	tbn_gs = mat3(tangent, bitangent, normal);





	uv_gs = interpolate2D(uv_te[0], uv_te[1],uv_te[2]);
	pos_gs = interpolate3D(pos_te[0],pos_te[1],pos_te[2]);







	//gl_Position = projection * view * vec4(pos,1.0f);//vec4(pos_gs,1.0f);
	gl_Position = projection * view * vec4(pos_gs,1.0f);


} 