#version 440 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec4 spatial; // x,y,z position + scale factor
layout (location = 2) in vec4 colour;  // r,g,b,a

uniform mat4 view;
uniform mat4 projection; // TODO: combine view and projection

out vec2  uv_fs;
out vec3 pos_fs;
out mat3 tbn_fs;
out vec4 col_fs;

// TODO: forward colour to fragment shader and combine with albedo!
void main() {
   // particle attributes:
   float particle_scale = spatial.w;
   vec3  particle_coord = spatial.xyz;

   // (by mapping from local space [-0.5,+0.5] range to UV space [0,1])
   uv_fs = vertex.xy + vec2( 0.5f, 0.5f );

   // extract right vector and up vector from view matrix:
   // we use them because the billboard will always face the camera
   // so we can re-use data from view instead of doing calculations
   // (the orientation is done @ pos_fs below; and they're used for the TBN matrix)
   vec3  view_right = { view[0][0], view[1][0], view[2][0] };
   vec3  view_up    = { view[0][1], view[1][1], view[2][1] };

   vec3  tangent    = view_up;
   vec3  bitangent  = view_right;
   vec3  normal     = cross( view_right, view_up );

   // matrix for normals calculation:
   tbn_fs      = mat3( tangent, bitangent, normal );

   // world space coordinate (scale the vertex position relative to the particle center coordinate by the scaling factor)
   pos_fs      = particle_coord
               + view_right * vertex.x * particle_scale
               + view_up    * vertex.y * particle_scale;

   col_fs      = colour; // forward colour to fragment shader

   gl_Position = projection * view * vec4( pos_fs, 1.0f );
}
