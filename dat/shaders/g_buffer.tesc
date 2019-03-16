



#version 430 core
layout (vertices = 3) out;

//the LoD function
float level (vec4 poz1, vec4 poz2){
    float lod=1;
	float d=distance(poz1, poz2);
	if(d<10) lod=10;
	if(10<=d && d<30) lod=5;
	if(30<=d && d<50) lod=2;
	if(50<=d) lod=1;

	return lod;
}

void main(void){

    if( gl_InvocationID == 0){
		vec3 d1=gl_in[1].gl_Position.xyz+(gl_in[2].gl_Position.xyz-gl_in[1].gl_Position.xyz)/2;
		vec3 d2=gl_in[0].gl_Position.xyz+(gl_in[2].gl_Position.xyz-gl_in[0].gl_Position.xyz)/2;
		vec3 d3=gl_in[0].gl_Position.xyz+(gl_in[1].gl_Position.xyz-gl_in[0].gl_Position.xyz)/2;
		
		float e0=level(vec4(d1,1.0),vec4(eye_position,1.0));
		float e1=level(vec4(d2,1.0),vec4(eye_position,1.0));
		float e2=level(vec4(d3,1.0),vec4(eye_position,1.0));
		float m=min(e0,min(e1,e2));
		
		gl_TessLevelInner[0] = floor((min(e0,min(e1,e2))+max(e0,max(e1,e2)))/2);
		gl_TessLevelOuter[0] = e0;
		gl_TessLevelOuter[1] = e1;
		gl_TessLevelOuter[2] = e2;
	}
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}





















#version 430 core
//you need to specify the OpenGL version, of course
//specify the number of vertices per patch
layout (vertices = 3) out;
void main(void){
    if (gl_InvocationID == 0){
        gl_TessLevelInner[0] = 7.0;
        gl_TessLevelOuter[0] = 2.0;
        gl_TessLevelOuter[1] = 3.0;
        gl_TessLevelOuter[2] = 7.0;

//in case of quad, you have to specify both gl_TessLevelInner[1] and //gl_TessLevelOuter[3]
    } 
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}








#version 410 core
// define the number of CPs in the output patch
layout (vertices = 3) out;
uniform vec3 gEyeWorldPos;

// attributes of the input CPs
in vec3 WorldPos_CS_in[];
in vec2 TexCoord_CS_in[];
in vec3 Normal_CS_in[];

// attributes of the output CPs
out vec3 WorldPos_ES_in[];
out vec2 TexCoord_ES_in[];
out vec3 Normal_ES_in[];

float GetTessLevel(float Distance0, float Distance1)
{
    float AvgDistance = (Distance0 + Distance1) / 2.0;
    if (AvgDistance <= 2.0)		 { return 10.0;}
    else if (AvgDistance <= 5.0) {return 7.0; }       
    else						 { return 3.0;}
} 

void main()
{
    // Set the control points of the output patch
    TexCoord_ES_in[gl_InvocationID] = TexCoord_CS_in[gl_InvocationID];
    Normal_ES_in[gl_InvocationID] = Normal_CS_in[gl_InvocationID];
    WorldPos_ES_in[gl_InvocationID] = WorldPos_CS_in[gl_InvocationID];
	    // Calculate the distance from the camera to the three control points
    float EyeToVertexDistance0 = distance(gEyeWorldPos, WorldPos_ES_in[0]);
    float EyeToVertexDistance1 = distance(gEyeWorldPos, WorldPos_ES_in[1]);
    float EyeToVertexDistance2 = distance(gEyeWorldPos, WorldPos_ES_in[2]);

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
} 