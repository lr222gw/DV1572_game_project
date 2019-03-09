#include "ParticleSystem.h"


// TODO: rename 'Config::particle_max_count' to make it more obvious that the limit is on a per ParticleSystem basis

// sorts the particles based on distance to the camera so that transparency handles properly.
void ParticleSystem::_sort_back_to_front( Vec3 const &view_position ) { // @TODO_TAG{swizzle}
   std::sort(  _particles.data,
              &_particles.data[_particles.count],
              [&view_position] ( Data::Particle &lhs, Data::Particle &rhs ) {
                                  return  lhs.time_ms_left > 0.0f
                                       && length( Vec3{lhs.spatial} - view_position )
                                        > length( Vec3{rhs.spatial} - view_position );
                               } );
}

// translates the CPU-side particle data (AoS) to VBO data (contiguous; SoA)
void ParticleSystem::_update_vbo_data() {
   assert( _particles.count < Data::capacity && "Out of bounds!" );
   for ( auto i = 0;  i < _particles.count;  ++i ) {
      _colour_vbo_data[i][0] = _particles.data[i].colour[0]; // red
      _colour_vbo_data[i][1] = _particles.data[i].colour[1]; // green
      _colour_vbo_data[i][2] = _particles.data[i].colour[2]; // blue
      _colour_vbo_data[i][3] = _particles.data[i].colour[3]; // alpha

      auto const &spatial            = _particles.data[i].spatial;
      auto const &transformed_coords = Vec3( _transform.matrix * Vec4( Vec3(spatial), 1.0f ) );
      // auto const &axis_scales        = _transform.get_scale();
      // auto const &transformed_scale  = ( axis_scales[0]
      //                                  + axis_scales[1]
      //                                  + axis_scales[2] ) / 3 * spatial.w;

      _spatial_vbo_data[i] = Vec4( transformed_coords,  spatial.w ); // position + scale
   }
}


// Static initialization method for static constant '_billboard_vbo_loc'
GLuint ParticleSystem::_generate_billboard_vbo() {
   static GLuint location;
   glBindVertexArray( _vao_loc );

   glGenBuffers( 1,                                        // number of buffers to generate
                 &location );                              // pointer to location to store buffer location
   glBindBuffer( GL_ARRAY_BUFFER,                          // target
                 location );                               // location
   glBufferData( GL_ARRAY_BUFFER,                          // target
                 sizeof(ParticleSystem::_vertex_vbo_data), // size
                 &ParticleSystem::_vertex_vbo_data,        // pointer to data
                 GL_STATIC_DRAW );                         // usage
   return location;
}

void ParticleSystem::_partition() {
   // TODO: combine with sorting
   auto new_end = std::partition(  _particles.data,
                                  &_particles.data[_particles.count],
                                  [] ( Data::Particle const& p ) {
                                     return p.time_ms_left > 0.0f;
                                  } );
   _particles.count = new_end - _particles.data;
}


// Default constructor
ParticleSystem::ParticleSystem( Transform              &&transform,
                                TextureSet               textures,
                                UpdateAlgorithm          updater,
                                InitializationAlgorithm  initializer ):
   _is_running        ( false                     ),
   _transform         ( std::move(transform)      ),
   _initializer       ( initializer               ),
   _updater           ( updater                   ),
   _textures          ( textures                  )
{
// Initialize particle data
   _initializer( _particles );
   _partition();

   glGenVertexArrays( 1, &_vao_loc );
   glBindVertexArray( _vao_loc );

   _billboard_vbo_loc = _generate_billboard_vbo(); // must be called after VAO is generated

// TODO: handle textures

// (Note:  The billboard vertex data VBO is generated and set statically; see further above.)

// Create VBO to contain positional data:

   //    This VBO will contain a list of center positions as well as the scaling
   //    factors for all 'n < particle_max_count' particles in the particle system.

   glGenBuffers( 1,                                                // number of buffers to generate
                 &_spatial_vbo_loc );                              // pointer to location to store buffer location
   glBindBuffer( GL_ARRAY_BUFFER,                                  // target
                 _spatial_vbo_loc );                               // location
   glBufferData( GL_ARRAY_BUFFER,                                  // target
                 sizeof(GLfloat) * 4 * Config::particle_max_count, // size
                 nullptr,                                          // pointer to data (will be set each frame)
                 GL_STREAM_DRAW );                                 // usage

// Create VBO to contain colour data:

   //    This VBO will contain a list of colour data (including alpha) for
   //    all 'n < particle_max_count' particles in the particle system.

   glGenBuffers( 1,                                                // number of buffers to generate
                 &_colour_vbo_loc );                               // pointer to location to store buffer location
   glBindBuffer( GL_ARRAY_BUFFER,                                  // target
                 _colour_vbo_loc );                                // location
   glBufferData( GL_ARRAY_BUFFER,                                  // target
                 sizeof(GLubyte) * 4 * Config::particle_max_count, // size
                 nullptr,                                          // pointer to data (will be set each frame)
                 GL_STREAM_DRAW );                                 // usage



   glBindVertexArray( 0 );
}


// Destructor
ParticleSystem::~ParticleSystem() {
   // TODO: unbind stuff
}


// Updater
void ParticleSystem::update( Float32 delta_time_ms ) {
   if ( !_is_running ) return;

// Run update algorithm to update particle data:
   _updater( _particles, delta_time_ms );
   _partition();
}


// Drawer
void ParticleSystem::draw( Vec3 const &viewport_position,  ShaderProgram &shader_program ) {
   _sort_back_to_front( viewport_position ); // necessary to avoid transparency issues
   _update_vbo_data();

// Bind shader program:
   shader_program.use();

   glBindVertexArray( _vao_loc );

   glEnable( GL_BLEND );

   TextureSet::ScopedBindGuard pin { _textures, shader_program }; // RAII

   // (Note: We're using instancing to speed up rendering;
   //        other alternatives include using a big VBO or emitting faces in a geometry shader.)


// Orphan and then update spatial VBO:
   glBindBuffer(    GL_ARRAY_BUFFER,                                  // target
                    _spatial_vbo_loc );                               // location
   glBufferData(    GL_ARRAY_BUFFER,                                  // target
                    sizeof(GLfloat) * 4 * Config::particle_max_count, // size
                    nullptr,                                          // data pointer
                    GL_STREAM_DRAW );                                 // usage
   glBufferSubData( GL_ARRAY_BUFFER,                                  // target
                    0,                                                // stride offset
                    sizeof(GLfloat) * 4 * Config::particle_max_count, // size
                    _spatial_vbo_data );                              // data pointer

// Orphan and then update colour VBO:
   glBindBuffer(    GL_ARRAY_BUFFER,                                  // target
                    _colour_vbo_loc );                                // location
   glBufferData(    GL_ARRAY_BUFFER,                                  // target
                    sizeof(GLubyte) * 4 * Config::particle_max_count, // size
                    nullptr,                                          // data pointer
                    GL_STREAM_DRAW );                                 // usage
   glBufferSubData( GL_ARRAY_BUFFER,                                  // target
                    0,                                                // stride offset
                    sizeof(GLubyte) * 4 * Config::particle_max_count, // size
                    _colour_vbo_data );

// Bind vertex VBO:
   glEnableVertexAttribArray( 0 );                  // attribute layout number
   glBindBuffer(              GL_ARRAY_BUFFER,      // target
                              _billboard_vbo_loc ); // location
   glVertexAttribPointer(     0,                    // attribute layout number
                              3,                    // size per vertex (TODO!)
                              GL_FLOAT,             // type of data
                              GL_FALSE,             // not normalized
                              0,                    // stride offset
                              (void*)0 );           // array buffer offset (TODO: nullptr?)

// Bind spatial VBO:
   glEnableVertexAttribArray( 1 );                  // attribute layout number
   glBindBuffer(              GL_ARRAY_BUFFER,      // target
                              _spatial_vbo_loc );   // location
   glVertexAttribPointer(     1,                    // attribute layout number
                              4,                    // size per vertex (X, Y, Z, and Scale)
                              GL_FLOAT,             // type of data
                              GL_FALSE,             // not normalized
                              0,                    // stride offset
                              (void*)0 );           // array buffer offset (TODO: nullptr?)

// Bind colour VBO:
   glEnableVertexAttribArray( 2 );                  // attribute layout number
   glBindBuffer(              GL_ARRAY_BUFFER,      // target
                              _colour_vbo_loc );    // location
   glVertexAttribPointer(     2,                    // attribute layout number
                              4,                    // size per vertex (Red, Green, Blue, and Alpha)
                              GL_UNSIGNED_BYTE,     // type of data
                              GL_TRUE,              // normalized (accessible as vec4 in shader)
                              0,                    // stride offset
                              (void*)0 );           // array buffer offset (TODO: nullptr?)


// Draw billboard instances:
   glVertexAttribDivisor( 0,                        // attribute layout number  (0 = vertex VBO)
                          0 );                      // stride per instance      (none; all billboards use a universal vertex data)
   glVertexAttribDivisor( 1,                        // attribute layout number  (1 = spatial VBO)
                          1 );                      // stride per instance      (one position & scale per billboard)
   glVertexAttribDivisor( 2,                        // attribute layout number  (2 = colour VBO)
                          1 );                      // stride per instance      (one colour per billboard)


   //glEnable(GL_BLEND);
   //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   //glBlendFunci()
   glDrawArraysInstanced( GL_TRIANGLE_STRIP,        // draw mode
                          0,                        // first index
                          4,                        // indices to draw
                          _particles.count );       // instance count
   //glDisable(GL_BLEND);

// Clean-up:

   // bind OpenGL to standard values
   glBindVertexArray( 0 ); // TODO: find way to wrap with RAII
}

void ParticleSystem::start() {
   _is_running = true;
}

Bool ParticleSystem::toggle() {
   return (_is_running = !_is_running);
}

void ParticleSystem::stop() {
   _is_running = false;
}


void ParticleSystem::set_state( Bool state ) {
   _is_running = state;
}

Bool ParticleSystem::get_state() const {
   return _is_running;
}

void ParticleSystem::set_textures( TextureSet textures ) {
   _textures = textures;
}

TextureSet ParticleSystem::get_textures() const {
   return _textures;
}

void ParticleSystem::transform( Transform const &transform ) {
   _transform *= transform;
}

void ParticleSystem::set_transform( Transform const &transform ) {
   _transform = transform;
}

Transform const & ParticleSystem::get_transform() const {
   return _transform;
}

void ParticleSystem::Data::add( Particle p ) {
   if ( count < capacity )
      data[count] = std::move( p );
   ++count;
}
