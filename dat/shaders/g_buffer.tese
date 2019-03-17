#version 440 core

layout(triangles, equal_spacing, ccw) in; //TODO: Clockwise or Counter Clockwise?

uniform mat4 view;
uniform mat4 projection;
uniform sampler2D tex_disp;
uniform sampler2D tex_norm;
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

	vec3 tangent = interpolate3D(tbn_te[0][0],tbn_te[1][0],tbn_te[2][0]);
	
	vec3 bitangent = interpolate3D(tbn_te[0][1],tbn_te[1][1],tbn_te[2][1]);
	
	vec3 normal = interpolate3D(tbn_te[0][2],tbn_te[1][2],tbn_te[2][2]);
	
	tbn_gs = mat3(tangent, bitangent, normal);


	uv_gs = interpolate2D(uv_te[0], uv_te[1],uv_te[2]);
	pos_gs = (gl_TessCoord.x*gl_in[0].gl_Position+gl_TessCoord.y*gl_in[1].gl_Position+gl_TessCoord.z* gl_in[2].gl_Position).xyz;//interpolate3D(pos_te[0],pos_te[1],pos_te[2]);
	//pos_gs = interpolate3D(pos_te[0],pos_te[1],pos_te[2]);

	//vec4 Normal = texture(tex_disp, gl_TessCoord.xy);
	vec4 displacement = texture(tex_disp, uv_gs);

	//vec3 tex_disp_val = texture( tex_disp, gl_TessCoord.xy ).rgb;
	//float avg_disp_val = (tex_disp_val.r + tex_disp_val.g + tex_disp_val.b) / 3;
	//
	//float heterogenous_disp_val = avg_disp_val / 128 - 1.0f;

	pos_gs += normal * displacement.xyz * 15;



	//gl_Position = projection * view * vec4(pos,1.0f);//vec4(pos_gs,1.0f);
	gl_Position = projection * view * vec4(pos_gs,1.0f);


} 