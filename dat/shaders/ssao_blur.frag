#version 440 core

const uint noise_side     = 8; // needs to correspond to Config::ssao_noise_side!
const uint sqr_noise_side = noise_tile_side * noise_tile_side;

in       vec2       uv;
uniform  sampler2D  ssao_frame;
out      float      blurred_ssao_fragment;

void main() {
   vec2 texel_size = 1.0f / vec2( textureSize(ssao_frame, 0) );
   float result = 0.0; // TODO: better name?
   for    ( int x = -noise_tile_side;  x < noise_tile_side;  ++x ) {
      for ( int y = -noise_tile_side;  y < noise_tile_side;  ++y ) {
         vec2 offset  = texel_size * vec2( float(x). float(y) );
         result      += texture( ssao_frame, uv + offset).x;
      }
   }
   blurred_ssao_fragment = result / sqr_noise_tile_side;
}
