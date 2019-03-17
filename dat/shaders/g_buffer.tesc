#version 440 core

// Number of Controlpoints per patch
layout (vertices = 4) out; //TODO: More vertrices? 
uniform vec3 view_pos;

// input data per Controlpoint
in vec3 pos_tc[];
in vec2 uv_tc[];
in mat3 tbn_tc[]; 


// output data per Controlpoint
out vec3 pos_te[];
out vec2 uv_te[];
out mat3 tbn_te[];

float distance_to_tesslevel(float first, float second)
{
	//Decides the level of detail based on distance to campos
    float average = (first + second) * 0.5;
	if (average <= 1.0)		 { return 20.0;}
    else if (average <= 2.0) { return 15.0;}
    else if (average <= 6.0) { return 10.0;}       
	else if (average <= 10.0){ return 5.0; }
	else if (average <= 14.0){ return 4.0; }
    else					 { return 2.0; }
} 

void main()
{
    // Set the control points of the output patch
    pos_te[gl_InvocationID] = pos_tc[gl_InvocationID]; //TODO: use gl_out[ gl_InvocationID].gl_Position Or in Evalshader? 
    uv_te[gl_InvocationID]  = uv_tc[gl_InvocationID];
    tbn_te[gl_InvocationID] = tbn_tc[gl_InvocationID];

	// Distance from camera to the ControlPoints of our patch
    float cam_to_left_side		= distance(view_pos, pos_tc[0]);
    float cam_to_bottom_side	= distance(view_pos, pos_tc[1]);
    float cam_to_right_side		= distance(view_pos, pos_tc[2]);
	float cam_to_top_side		= distance(view_pos, pos_tc[3]);

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] = 4;//distance_to_tesslevel(cam_to_bottom_side,	cam_to_right_side	);
    gl_TessLevelOuter[1] = 4;//distance_to_tesslevel(cam_to_right_side,		cam_to_top_side		);
    gl_TessLevelOuter[2] = 4;//distance_to_tesslevel(cam_to_top_side,		cam_to_left_side	);
	gl_TessLevelOuter[3] = 4;//distance_to_tesslevel(cam_to_left_side,		cam_to_bottom_side	);

	//inner tessellation level are based on the outer tessellated levels for the corresponding side (horizontal and vertical)
    gl_TessLevelInner[0] = 4;	 //(gl_TessLevelOuter[0] + gl_TessLevelOuter[2])/
	gl_TessLevelInner[1] = 4;	 //(gl_TessLevelOuter[1] + gl_TessLevelOuter[3])/
} 