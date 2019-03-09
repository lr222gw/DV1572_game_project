#pragma once

#include "misc/defs.h"
#include "Config.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "Transform.h"

// TODO:  implement animations
// TODO:  implement optional callback functions for particle creation, particle destruction, etc
// TODO:  allow for instancing of particle data?
//        (E.g. ParticleSystemInstance that wraps a ParticleSystem and a Transform)

// SoA code:

      // RGBA     colour           [capacity] = {}; // R, G, B, Alpha    4n bytes
      // Vec4     spatial          [capacity] = {}; // X, Y, Z, Scale   16n bytes
      // Vec3     velocity_m_per_s [capacity] = {}; // X, Y, Z          12n bytes
      // Float32  time_ms_left     [capacity] = {}; //                   4n bytes
      // Float32  mass_kg          [capacity] = {}; //                   4n bytes
      // TODO:  ponder further useful attributes

      // void swap( Uint32 index_a, Uint32 index_b ) {
      //    assert( MAX(index_a, index_b) < capacity  &&  "Index out of bounds!" );
      //    std::swap(           colour [index_a],              colour [index_b] );
      //    std::swap(          spatial [index_a],             spatial [index_b] );
      //    std::swap( velocity_m_per_s [index_a],    velocity_m_per_s [index_b] );
      //    std::swap(     time_ms_left [index_a],        time_ms_left [index_b] );
      //    std::swap(          mass_kg [index_a],             mass_kg [index_b] );
      // }




class ParticleSystem {
public:
   using Rgba = GLubyte[4];
   struct Data { /* AoS CPU model of a ParticleSystem's data */
      // TODO: make into class and make some stuff private
      struct Particle {
         Uvec4    colour           = {  0,   0,   0,  0xFF }; // R, G, B, Alpha    4 bytes
         Vec4     spatial          = { .0f, .0f, .0f, 1.0f }; // X, Y, Z, Scale   16 bytes
         Vec3     velocity_m_per_s = { .0f, .0f, .0f       }; // X, Y, Z          12 bytes
         Float32  time_ms_left     = { .0f                 }; //                   4 bytes
         Float32  mass_kg          = { .0f                 }; //                   4 bytes
         // TODO:  ponder further useful attributes           // total size:      40 bytes
      };

      // TODO: commpute velocity in algorithm instead? if so, remove from Particle data
      // or set in algorithm but update in class?

      void add( Particle );

      static Uint32 constexpr capacity       = { Config::particle_max_count };
      Particle                data[capacity] = {};
      Uint32                  count          = { 0 };
   };

   using InitializationAlgorithm = std::function< void( Data & ) >;
   using UpdateAlgorithm         = std::function< void( Data &, Float32 ) >;

   ParticleSystem( Transform &&, TextureSet, UpdateAlgorithm, InitializationAlgorithm = [](Data&){} ); // TODO: add callbacks
   ParticleSystem( ParticleSystem const &  ) = delete;
   ParticleSystem( ParticleSystem       && ) = delete;
  ~ParticleSystem();
   ParticleSystem & operator=( ParticleSystem const &  ) = delete;
   ParticleSystem & operator=( ParticleSystem       && ) = delete;

   void update( Float32 delta_time_ms );
   void draw( Vec3 const &viewport_position, ShaderProgram & );

   void start();
   Bool toggle();
   void stop();

   void set_state( Bool );
   Bool get_state() const;

   void       set_textures( TextureSet );
   TextureSet get_textures() const;

   void              transform(     Transform const & );
   void              set_transform( Transform const & );
   Transform const & get_transform() const;

private:
// private member functions:
   // sorts the particle data based on particle distance to the camera so that transparency handles properly
   void _sort_back_to_front( Vec3 const &view_position );
   // used to initialize '_billboard_vbo_loc'
   [[nodiscard]] GLuint _generate_billboard_vbo();
   // translates the CPU-side particle data (AoS) to VBO data (contiguous; SoA)
   void _update_vbo_data();
   // partitions the particle data & updates the particle count accordingly
   void _partition();

// private member data:
   // defining traits:
   Bool                         _is_running;        // particle system current activity state
   Transform                    _transform;         // relative location, rotation, scale
   Data                         _particles;         // particle-specific data
   InitializationAlgorithm      _initializer;       // contains logic for particle initialization (initial state)
   UpdateAlgorithm              _updater;           // contains logic for particle updating       (state changes)
   TextureSet                   _textures;          // texture data
   // VAO
   GLuint                       _vao_loc;           // location of vao
   // VBO locations:
   GLuint                       _spatial_vbo_loc;   // location of 3D position + uniform scale factor VBO
   GLuint                       _colour_vbo_loc;    // location of colour and alpha VBO
   GLuint                       _billboard_vbo_loc; // initialized in source (TODO: find way to make static without issues)
   // VBO data in contiguous format:
   Vec4                         _spatial_vbo_data [Data::capacity]; // maintained by _update_vbo_data() via update()
   Rgba                         _colour_vbo_data  [Data::capacity]; // maintained by _update_vbo_data() via update()
   static GLfloat constexpr     _vertex_vbo_data[] = {              // this quad vertex data will be shared between all particles
       -0.5f,  -0.5f,  +0.0f,
       +0.5f,  -0.5f,  +0.0f,
       -0.5f,  +0.5f,  +0.0f,
       +0.5f,  +0.5f,  +0.0f
   };
};
