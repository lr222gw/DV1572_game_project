#pragma once

#include "misc/defs.h"
#include "Config.h"
#include "ShaderProgram.h"
#include "Viewport.h"

class LightDebugger; // forward declaration

class Circle {
   friend class LightDebugger;
public:
   // TODO: add Transforms

   Circle( Vec3             world_position = { 0.0f, 0.0f, 0.0f },
           Array<GLubyte,3> rgb_color      = { 255U,  255U,  0U },
           Float32          radius         = 1.0f,
           Uint32           num_segments   = 32U,
           Bool             is_hidden      = false
   ):
      _is_hidden       ( is_hidden      ),
      _world_position  ( world_position ),
      _rgb_color       ( rgb_color      ),
      _radius          ( radius         )
   {
      _generate_mesh( num_segments );

      glGenVertexArrays( 1, &_vao );
      glBindVertexArray( _vao );

      glGenBuffers( 1, &_vbo );
      glBindBuffer( GL_ARRAY_BUFFER, _vbo );
      glBufferData( GL_ARRAY_BUFFER, sizeof(_vertices[0])*_vertices.size(), &_vertices[0], GL_STATIC_DRAW );

      glEnableVertexAttribArray( 0 );
      glVertexAttribPointer( _vbo,        // the attribute we want to configure
                             3,           // size (one for each 3D dimensional axis)
                             GL_FLOAT,    // type
                             GL_FALSE,    // not normalized
                             0,           // no stride
                             nullptr );   // array buffer offset

      glBindVertexArray(0);

      // glDeleteBuffers( 1, _vbo );
      // glDeleteBuffers( 1, _vao );
   }

   #define LINE_ALPHA 1.0f
   #define LINE_GLOW  1.0f

   // draws circle facing the camera
   void draw( Viewport &view, SharedPtr<ShaderProgram> shader_program ) {
      if ( _is_hidden ) return;

      std::cout << "[DEBUG]\tPrinting light circle.\n";

      auto point = 1U;
      for ( auto &e : _vertices )
         std::cout << "Point " <<  point++ << " @ (x:" << e[0] << ", y:" << e[1] << ", z: " << e[2] << ")\n";

      shader_program->use();

      view.bind_shader_program( shader_program );

      //auto t = Transform { _world_position,
      //                      glm::lookAt( _world_position,
      //                                   view.get_view().get_position(),
      //                                   Vec3(0.0f, 1.0f, 0.0f) ),
      //                      Vec3( _radius ) };

      Transform t;
      // t.set_scale( Vec3( _radius / 4 ) );
      // t.set_position( _world_position );
      t.set_scale( Vec3( 2.0 ) );
      t.set_position( Vec3(1,9,1) );

      auto rgba = Vec4( Float32(_rgb_color[0]/255.0f),
                        Float32(_rgb_color[1]/255.0f),
                        Float32(_rgb_color[2]/255.0f),
                        LINE_ALPHA );
      glUniform4fv( glGetUniformLocation( shader_program->get_location(), "rgba"),
                    1,
                    glm::value_ptr(rgba) );

      auto glow = LINE_GLOW;
      glUniform1f( glGetUniformLocation( shader_program->get_location(), "glow"),
                   glow );

      // prepare for drawing:
      glColor3ub( 0, 255, 255 );
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

      // draw
      //glVertexPointer( 3, GL_FLOAT, 0, &_vertices[i*360]);
      //glDrawElements( GL_TRIANGLE_FAN,
      //                _vertices.size(),
      //                GL_UNSIGNED_INT, // float?
      //                0 );
      glUniformMatrix4fv( shader_program->get_transform_location(),
                          1,
                          GL_FALSE,
                          &(t.matrix[0][0]) );


      glBindVertexArray( _vao );
//    glEnableVertexAttribArray( 0 );
      glDrawArrays( GL_LINE_LOOP, 0, _vertices.size() ); // draw the points and fill it in

      // restore to normal:
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
      glColor3ub( 255, 255, 255 );
      //glDisableVertexAttribArray( 0 );
      glBindVertexArray( 0 );
   }

   Bool is_hidden() const {
      return _is_hidden;
   }

   void set_hidden( Bool state = true ) {
      _is_hidden = state;
   }

   void set_position( Vec3 world_position ) {
      _world_position = std::move( world_position );
   }

   Vec3 get_position() const {
      return _world_position;
   }

   void set_color( Array<GLubyte,3> rgb ) {
      _rgb_color = rgb;
   }

   Array<GLubyte,3> get_color() const {
      return _rgb_color;
   }

   void set_radius( Float32 radius ) {
      _radius = radius;
   }

   Float32 get_radius() const {
      return _radius;
   }

   Uint32 get_number_of_segments() const {
      return _vertices.size();
   }

private:
   void _generate_mesh( Uint32 num_segments ) {
      _vertices.clear();
      _vertices.reserve( num_segments );

      Float32 theta = 2.0f * 3.1415926f / (Float32)num_segments;

      Mat2 rot_mat  { cosf(theta), -sinf(theta),
                      sinf(theta),  cosf(theta) };

      Vec2 xy { 1.0f, 0.0f };

      for ( Uint32 i = 0;  i < num_segments;  ++i ) {
         xy = rot_mat * xy;
         _vertices.emplace_back( xy.x, 0.0f, xy.y );
      }
   }

   Bool              _is_hidden;
   GLuint            _vao;
   GLuint            _vbo;
   Vector<Vec3>      _vertices;
   Vec3              _world_position;
   Array<GLubyte,3>  _rgb_color;
   Float32           _radius;
};
