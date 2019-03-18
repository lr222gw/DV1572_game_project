#pragma once
#include "misc/defs.h"
#include "Config.h"
#include <random>

class SuperSampledAmbientOcclusion {
public:
   SuperSampledAmbientOcclusion() {
      static Bool is_initialized = false;
      if ( !is_initialized )
         init();
   }

static void init() {
   static std::random_device                       rd           {};
   static std::mt19937                             rng          { rd() }; // Mersienne Twister random number generator
   static std::uniform_real_distribution<Float32>  dist         { -1.0f, +1.0f };
   static Array<Vec3,Config::ssao_kernel_count>    ssao_kernel  {};

   // SSAO kernel
   auto kernel_num = 0U;
   for ( auto &e : ssao_kernel ) {
      Vec3 ssao_sample { dist(rng),                 // X:  -1.0 ~ 1.0
                         dist(rng),                 // Y:  -1.0 ~ 1.0
                         dist(rng) * 0.5f + 1.0f }; // Z:   0.0 ~ 1.0
      ssao_sample = glm::normalize( ssao_sample ) * (dist(rng) * 0.5f + 1.0f);
      auto lerp   = []( auto lo, auto hi, auto factor ) {
                       return factor * (hi - lo) + lo;
                    };
      Float32 scale  = kernel_num * Config::ssao_scale_growth_factor;
      scale          = lerp( .1f, 1.0f, scale*scale ); // interpolate linearly
      ssao_sample   *= scale;
      e = ssao_sample;
   }

   // SSAO noise
   constexpr Uint32 noise_values = Config::ssao_noise_side * Config::ssao_noise_side;
   Array<Vec3, noise_values> ssao_noise {};
   for ( auto &e : ssao_noise )
      e = Vec3( dist(rng), dist(rng), .0f );

   Uint32 noise_tex_loc;
   glGenTextures(   1, &noise_tex_loc );
   glBindTexture(   GL_TEXTURE_2D, noise_tex_loc );
   glTexImage2D(    GL_TEXTURE_2D, 0, GL_RGB16F, Config::ssao_noise_side, Config::ssao_noise_side, 0, GL_RGB, GL_FLOAT, &ssao_noise[0] );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT  );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT  );
}

Uint32 get_noise_texture_location() const { return _noise_tex_loc; }

private:
   static Uint32 _noise_tex_loc;
};
