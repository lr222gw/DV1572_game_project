#version 440 core

// needed for level-of-detail

// number of control points per patch
layout (vertices = 3) out;
uniform vec3 view_pos;

// tess_percent; boost Tessellation Level by adding a value from 0.0f to 1.0f
uniform float tess_percent;

// input data per control points
in vec3 pos_tc[];
in vec2  uv_tc[];
in mat3 tbn_tc[];

// output data per control points
out vec3 pos_te[];
out vec2  uv_te[];
out mat3 tbn_te[];

float distance_to_tesslevel( float dist ) {
   // Decides the level of detail based on distance to campos
   // float average = (first + second) * 0.5;
   // TODO: convert into a function
   // if      (dist <=  10.0) { return max(gl_MaxTessGenLevel*min(1.00f + tess_percent, 1.0f), 1.0f ); }
   // else if (dist <=  15.0) { return max(gl_MaxTessGenLevel*min(0.95f + tess_percent, 1.0f), 1.0f ); }
   // else if (dist <=  30.0) { return max(gl_MaxTessGenLevel*min(0.90f + tess_percent, 1.0f), 1.0f ); }
   // else if (dist <=  35.0) { return max(gl_MaxTessGenLevel*min(0.85f + tess_percent, 1.0f), 1.0f ); }
   // else if (dist <=  40.0) { return max(gl_MaxTessGenLevel*min(0.80f + tess_percent, 1.0f), 1.0f ); }
   // else if (dist <=  50.0) { return max(gl_MaxTessGenLevel*min(0.60f + tess_percent, 1.0f), 1.0f ); }
   // else if (dist <=  75.0) { return max(gl_MaxTessGenLevel*min(0.45f + tess_percent, 1.0f), 1.0f ); }
   // else if (dist <=  95.0) { return max(gl_MaxTessGenLevel*min(0.20f + tess_percent, 1.0f), 1.0f ); }
   // else if (dist <= 120.0) { return max(gl_MaxTessGenLevel*min(0.05f + tess_percent, 1.0f), 1.0f ); }
   // else                    { return 1;                                                              }

   // NOTE: gl_MaxTessGenLevel: is the maximum level of tessellatioon (32 or 64, usually)
   const float max_dist    = 80; // TODO: find balance between max_dist here and tess_percent (in CPU)
   float       dist_factor = max_dist/dist * max_dist/dist;
   return max( gl_MaxTessGenLevel * ( min(dist_factor, 1.0) + tess_percent), 1.0);
}

void main() {
   if ( gl_InvocationID == 0 ) {
      // Set the control points of the output patch
      // Calculating Vertices between our triangles corners
      vec3 middle_0_1 = pos_tc[0] + (pos_tc[1]-pos_tc[0])/2; //Middle between vert 0 and 1
      vec3 middle_1_2 = pos_tc[1] + (pos_tc[2]-pos_tc[1])/2; //Middle between vert 1 and 2
      vec3 middle_2_0 = pos_tc[2] + (pos_tc[0]-pos_tc[2])/2; //Middle between vert 2 and 0

      // Distance from camera to the inbetweens of our Patch's ControlPoints
      float cam_to_0_1     = distance(view_pos, middle_0_1);
      float cam_to_1_2     = distance(view_pos, middle_1_2);
      float cam_to_2_0     = distance(view_pos, middle_2_0);
      float cam_to_center  = (cam_to_0_1 + cam_to_1_2 + cam_to_2_0)/3;

      // Set tessellation level based on distance from patch
      gl_TessLevelOuter[0] = distance_to_tesslevel( cam_to_1_2 );
      gl_TessLevelOuter[1] = distance_to_tesslevel( cam_to_2_0 );
      gl_TessLevelOuter[2] = distance_to_tesslevel( cam_to_0_1 );

      gl_TessLevelInner[0] = distance_to_tesslevel( cam_to_center );
   }

   // pass through:
   pos_te[gl_InvocationID] = pos_tc[gl_InvocationID];
    uv_te[gl_InvocationID] =  uv_tc[gl_InvocationID];
   tbn_te[gl_InvocationID] = tbn_tc[gl_InvocationID];
}
