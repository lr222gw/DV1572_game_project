#version 440 core

// Number of Controlpoints per patch
layout (vertices = 3) out; //TODO: More vertrices? 
uniform vec3 view_pos;

//tess_percent, boost Tessellation Level by adding a value from 0.0f to 1.0f
//Default should be 0
uniform float tess_percent; 

// input data per Controlpoint
in vec3 pos_tc[];
in vec2 uv_tc[];
in mat3 tbn_tc[]; 


// output data per Controlpoint
out vec3 pos_te[];
out vec2 uv_te[];
out mat3 tbn_te[];

float distance_to_tesslevel(float dist)
{
	//Decides the level of detail based on distance to campos
    //float average = (first + second) * 0.5;
	if (dist <= 10.0)	    { return max(gl_MaxTessGenLevel*min(1.0f  + tess_percent, 1.0f), 1.0f	);	}
    else if (dist <= 15.0)  { return max(gl_MaxTessGenLevel*min(0.95f + tess_percent, 1.0f), 1.0f	);	}		
    else if (dist <= 30.0)  { return max(gl_MaxTessGenLevel*min(0.90f + tess_percent, 1.0f), 1.0f	);	}       
	else if (dist <= 35.0)  { return max(gl_MaxTessGenLevel*min(0.85f + tess_percent, 1.0f), 1.0f	);  }		
	else if (dist <= 40.0)  { return max(gl_MaxTessGenLevel*min(0.80f + tess_percent, 1.0f), 1.0f	);	}		
	else if (dist <= 50.0)  { return max(gl_MaxTessGenLevel*min(0.60f + tess_percent, 1.0f), 1.0f	);	}	
	else if (dist <= 75.0)  { return max(gl_MaxTessGenLevel*min(0.45f + tess_percent, 1.0f), 1.0f	);	}   
	else if (dist <= 95.0)  { return max(gl_MaxTessGenLevel*min(0.20f + tess_percent, 1.0f), 1.0f	);  }	
	else if (dist <= 120.0) { return max(gl_MaxTessGenLevel*min(0.05f + tess_percent, 1.0f), 1.0f	);	}		
    else				    { return 1;																	}
} 

float level (vec4 poz1, vec4 poz2){
    float lod=1;
	float d=distance(poz1, poz2);
	if(d<10) lod=60;
	if(10<=d && d<30) lod=5;
	if(30<=d && d<50) lod=2;
	if(50<=d) lod=1;

	return lod;
}


void main()
{


	if (gl_InvocationID == 0){
		// Set the control points of the output patch
		
		//vec3 d1=pos_tc[1]+(pos_tc[2]-pos_tc[1])/2;
		//vec3 d2=pos_tc[0]+(pos_tc[2]-pos_tc[0])/2;
		//vec3 d3=pos_tc[0]+(pos_tc[1]-pos_tc[0])/2;

		//Calculating Vertices between our triangles corners
		vec3 middle_0_1 = pos_tc[0] + (pos_tc[1]-pos_tc[0])/2; //Middle between vert 0 and 1
		vec3 middle_1_2 = pos_tc[1] + (pos_tc[2]-pos_tc[1])/2; //Middle between vert 1 and 2
		vec3 middle_2_0 = pos_tc[2] + (pos_tc[0]-pos_tc[2])/2; //Middle between vert 2 and 0
		
		//float e0=level(vec4(d1,1.0),vec4(view_pos,1.0));
		//float e1=level(vec4(d2,1.0),vec4(view_pos,1.0));
		//float e2=level(vec4(d3,1.0),vec4(view_pos,1.0));
		//float m=min(e0,min(e1,e2));

		// Distance from camera to the inbetweens of our Patch's ControlPoints
		float cam_to_0_1		= distance(view_pos, middle_0_1);
		float cam_to_1_2		= distance(view_pos, middle_1_2);
		float cam_to_2_0		= distance(view_pos, middle_2_0);
		float cam_to_center		= (cam_to_0_1 + cam_to_1_2 + cam_to_2_0)/3;

		float max_dist = 5.0f;
		
		//gl_TessLevelInner[0] = floor((min(e0,min(e1,e2))+max(e0,max(e1,e2)))/2);
		//gl_TessLevelOuter[0] = e0;
		//gl_TessLevelOuter[1] = e1;
		//gl_TessLevelOuter[2] = e2;

		//float div = 0.5f;
		float div = 75.0f;






		gl_TessLevelOuter[0] = distance_to_tesslevel( cam_to_1_2		);
		gl_TessLevelOuter[1] = distance_to_tesslevel( cam_to_2_0		);
		gl_TessLevelOuter[2] = distance_to_tesslevel( cam_to_0_1		);
													  
		gl_TessLevelInner[0] = distance_to_tesslevel( cam_to_center		);





		// Calculate the tessellation levels
		//gl_TessLevelOuter[0] = max(mix(1,	gl_MaxTessGenLevel,  min(max_dist/(cam_to_0_1*div), 1 ) ),	1);//distance_to_tesslevel(cam_to_bottom_side,	cam_to_right_side	);
		//gl_TessLevelOuter[1] = max(mix(1,	gl_MaxTessGenLevel,  min(max_dist/(cam_to_1_2*div), 1 ) ),	1);//distance_to_tesslevel(cam_to_right_side,		cam_to_top_side		);
		//gl_TessLevelOuter[2] = max(mix(1,	gl_MaxTessGenLevel,  min(max_dist/(cam_to_2_0*div), 1 ) ),	1);//distance_to_tesslevel(cam_to_top_side,		cam_to_left_side	);
		//gl_TessLevelInner[0] = max(mix(1,	gl_MaxTessGenLevel,  min(max_dist/(cam_to_center*div),1 )),1);	 //(gl_TessLevelOuter[0] + gl_TessLevelOuter[2])/

		//gl_TessLevelOuter[0] = max(mix(	gl_MaxTessGenLevel, 1,  min((cam_to_0_1/div), 1 ) ),	1);//distance_to_tesslevel(cam_to_bottom_side,	cam_to_right_side	);
		//gl_TessLevelOuter[1] = max(mix(	gl_MaxTessGenLevel, 1,  min((cam_to_1_2/div), 1 ) ),	1);//distance_to_tesslevel(cam_to_right_side,		cam_to_top_side		
		//gl_TessLevelOuter[2] = max(mix(	gl_MaxTessGenLevel, 1,  min((cam_to_2_0/div), 1 ) ),	1);//distance_to_tesslevel(cam_to_top_side,		cam_to_left_side	);
		//
		//gl_TessLevelInner[0] = max(mix(	gl_MaxTessGenLevel, 1,  min((cam_to_center/div),1 )),1);	 //(gl_TessLevelOuter[0] + gl_TessLevelOuter[2])/

		//gl_TessLevelOuter[3] = 4;//distance_to_tesslevel(cam_to_left_side,		cam_to_bottom_side	);
		
		//inner tessellation level are based on the outer tessellated levels for the corresponding side (horizontal and vertical)
		//gl_TessLevelInner[1] = 4;	 //(gl_TessLevelOuter[1] + gl_TessLevelOuter[3])/
	}

	pos_te[gl_InvocationID] = pos_tc[gl_InvocationID];
	uv_te[gl_InvocationID]  = uv_tc[gl_InvocationID];
	tbn_te[gl_InvocationID] = tbn_tc[gl_InvocationID];

	//gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
} 