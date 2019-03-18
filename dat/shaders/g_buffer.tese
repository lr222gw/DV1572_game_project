#version 440 core
layout(triangles, equal_spacing, ccw) in; 

uniform mat4 view;
uniform mat4 projection;
uniform sampler2D tex_disp;
uniform sampler2D tex_norm;
uniform float displacement_factor;
 
in vec3 pos_te[];
in vec2 uv_te[];
in mat3 tbn_te[];

out vec3 pos_gs;
out vec2 uv_gs ;
out mat3 tbn_gs;

//Interpolation functions, gives us the corressponding vector (2D or 3D) based on 
//gl_TessCoord (from primitve generator)
vec3 interpol_tri_vec3(vec3 first_corner, vec3 second_corner, vec3 third_corner)
{
	vec3 interpol_vec3  = vec3(gl_TessCoord.x) * first_corner 
						+ vec3(gl_TessCoord.y) * second_corner 
						+ vec3(gl_TessCoord.z) * third_corner;
    return interpol_vec3;
}

vec2 interpol_tri_vec2(vec2 first_corner, vec2 second_corner, vec2 third_corner)
{
	vec2 interpol_vec2	= vec2(gl_TessCoord.x) * first_corner 
						+ vec2(gl_TessCoord.y) * second_corner 
						+ vec2(gl_TessCoord.z) * third_corner;

    return interpol_vec2;
} 

void main()
{
	//interpolate uv and position
	uv_gs = interpol_tri_vec2(uv_te[0], uv_te[1],uv_te[2]);	
	pos_gs = interpol_tri_vec3(pos_te[0],pos_te[1],pos_te[2]);
	//get interpolated tangent, bitangent and normal
	vec3 tangent = interpol_tri_vec3(tbn_te[0][0],tbn_te[1][0],tbn_te[2][0]);
	vec3 bitangent = interpol_tri_vec3(tbn_te[0][1],tbn_te[1][1],tbn_te[2][1]);	
	vec3 normal = interpol_tri_vec3(tbn_te[0][2],tbn_te[1][2],tbn_te[2][2]);
	
	//set outmatrix to new matrix with interpolated vectors
	tbn_gs = mat3(tangent, bitangent, normal);

	//sample displacementmap based on interpolated uv_coords
	vec4 displacement = texture(tex_disp, uv_gs );

	//Apply displacement on interpolated position
	vec3 halfvec = vec3(.5f,.5f,.5f); // halfvec makes our displacement value go up and down
	pos_gs += normal * ((displacement.xyz-halfvec)*2) * displacement_factor; //Multiply by 2 to get the right displacementLevel...

	gl_Position = projection * view * vec4(pos_gs,1.0f);


} 