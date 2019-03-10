#version 440 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec4 spatial; // x,y,z position + scale factor
layout (location = 2) in vec4 colour;  // r,g,b,a

uniform mat4 view;
uniform mat4 projection;    // TODO: combine view and projection

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
   vec3  view_right = { view[0][0], view[1][0], view[2][0] }; //
   vec3  view_up    = { view[0][1], view[1][1], view[2][1] }; //

   vec3  tangent    = view_up;                      // kontrollera normalisering om buggar
   vec3  bitangent  = view_right;                   // kontrollera normalisering om buggar
   vec3  normal     = cross( view_right, view_up ); // kontrollera normalisering om buggar

   // vec3 normal     = normalize( view_position - particle_coord );
   // vec3 tangent?   = cross( view_up, normal );
   // vec3 bitangent? = cross( tangent, normal );

   // elements needed to generate normals matrix:
   // vec3 tang = normalize( vec3(model_transform * vec4(tangent,   0.0) ) );
   // vec3 btan = normalize( vec3(model_transform * vec4(bitangent, 0.0) ) );
   // vec3 norm = normalize( vec3(model_transform * vec4(normal,    0.0) ) );

   // matrix for normals calculation:
   tbn_fs    = mat3( tangent, bitangent, normal );

   // world space coordinate:
   pos_fs    = particle_coord
             + view_right * vertex.x * particle_scale
             + view_up    * vertex.y * particle_scale;

   col_fs    = colour;

   //pos_fs = vec3( 0.5, 0.5, 0.5 );

   gl_Position = projection * view * vec4( pos_fs, 1.0f ); //vec4( pos_gs, 1.0f );
}
