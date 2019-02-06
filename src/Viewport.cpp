#include "Viewport.h"
#include "debug.h"

// TODO: l�s ViewPort -> Viewport

Viewport::Viewport(Vec3 position, GLFWwindow *window, Float32 fov_rad):
   _fov    ( fov_rad ),
   _aspect ( -1.0f   ),
   _window ( window  ),
   front   (1.0f,1.0f,1.0f)
{
   // TODO: bind _camera och uniform buffer f�r Mat4

   //_model = Mat4(1.0f);

   // validera
   _view = Transform( position );

   _update_aspect_ratio();

   _write_to_buffer();
}

void Viewport::_update_aspect_ratio() {
   glfwGetWindowSize( _window, &_width, &_height );
   Float32 aspect = Float32(_width) / Float32(_height);
   if ( aspect != _aspect ) {
      _aspect = aspect;

      _g_buffer_init(width, height);
      _generate_perspective();

      //TODO: Does this work? 
      glfwMakeContextCurrent(this->_window);
      glfwGetFramebufferSize(this->_window, &_width, &_height);
      glViewport(0, 0, _width, _height);
   }

}

void Viewport::_generate_perspective() {
   _projection = glm::perspective( _fov,
                                   _aspect,
                                   config::near_plane,
                                   config::far_plane );   
   _write_to_buffer();
}

// void Viewport::set_position(Vec3 new_position) {
//    _position = new_position;
//    _update_view_matrix();
// }
// 
// void Viewport::set_rotation(Vec3 new_rotation) {
//    _rotation = new_rotation;
//    _update_view_matrix();
// }

void Viewport::transform( Transform const &transform ) {
   _view *= transform;
   _write_to_buffer();
}

void Viewport::set_view( Transform const &transform ) {
   _view = transform;
   _write_to_buffer();
}

Transform Viewport::get_view() const {
   return _view;
}

GBufferData const &Viewport::get_g_buffer() const {
   return _g_buffer;
}

void Viewport::set_fov(Float32 fov_rad) {
   _fov = fov_rad;
   _projection = glm::perspective( _fov,
                                   config::aspect_ratio,
                                   config::near_plane,
                                   config::far_plane );
   _write_to_buffer();
}

void Viewport::bind_shader_program(ShaderProgram &shapro) {
   _location = shapro.getProgramLoc();
   _write_to_buffer();
}

// void Viewport::_update_view_matrix() {
   // _position = vec3 med x,y,z
   // _rotation = vec3 med rotation kring x, y, z i radianer
   // dvs.
   // _rotation.x = hur m�nga radianer roterar vi kring X-axeln
   // _rotation.y = hur m�nga radianer roterar vi kring Y-axeln
   // _rotation.z = hur m�nga radianer roterar vi kring Z-axeln

 /*/ den h�r anv�nder inte rotation
      _view = glm::lookAt( _position,
                           _position + Vec3{ 0, 0, -1 },
                           Vec3{ 0, 1, 0 } );
//*/

/// den h�r roterar med kvatern
//    glm::quat rotation_quaternion {Vec4( _rotation, 0.0f) };
// 
//    _view       = glm::toMat4(rotation_quaternion);;
//    _view[3][0] = _position.x;
//    _view[3][1] = _position.y;
//    _view[3][2] = _position.z;
// //*/
//    _write_to_buffer();
// }



void Viewport::update() {
   debug::view_mat4( _view.matrix, "viewport._view");
   _update_aspect_ratio();

}

void Viewport::_write_to_buffer() {
   // _camera = _projection * _view; // * _model; // validera?
   // glUniformMatrix4fv( _location, 1, GL_FALSE, glm::value_ptr(_camera) );
   //glUniformMatrix4fv( glGetUniformLocation(_location, "model"),      1, GL_FALSE, glm::value_ptr(_model)      );
   //glUniformMatrix4fv( glGetUniformLocation(_location, "model"),      1, GL_FALSE, &(_model.get_transform()[0][0]) );
   //glUniformMatrix4fv( glGetUniformLocation(_location, "view"),       1, GL_FALSE, glm::value_ptr(_view)       );
   glUniformMatrix4fv( glGetUniformLocation(_location, "view"),       1, GL_FALSE, &(_view.matrix[0][0]) );
   //glUniformMatrix4fv( glGetUniformLocation(_location, "projection"), 1, GL_FALSE, glm::value_ptr(_projection) );
   glUniformMatrix4fv( glGetUniformLocation(_location, "projection"), 1, GL_FALSE, &_projection[0][0] );
}

void Viewport::_g_buffer_init(float width, float height) {
  
   // specular  =  specular RGB (=xyz) + specular intensity (=w)
   // albedo    =  albedo RGBA(= xyzw)

   //TODO: extract Magnifier and minifier behavior to be changed in options
  
   if ( true == this->_g_buffer.is_set ) {
      //gBuffer init
      glGenFramebuffers( 1, &(this->_g_buffer.buffer_loc) );
      //gBuffer Texture attatchments init
      glGenTextures( 1, &(this->_g_buffer.pos_tex_loc) );
      glGenTextures( 1, &(this->_g_buffer.nor_tex_loc) );
      glGenTextures( 1, &(this->_g_buffer.spe_tex_loc) );
      glGenTextures( 1, &(this->_g_buffer.alb_tex_loc) );
   }

   glBindFramebuffer( GL_FRAMEBUFFER,
                      this->_g_buffer.buffer_loc );

// position texture for g-buffer:
   glBindTexture( GL_TEXTURE_2D,
                  this->_g_buffer.pos_tex_loc );

   glTexImage2D( GL_TEXTURE_2D,
                 0,
                 GL_RGB16F,
                 width,
                 height,
                 0,
                 GL_RGB,
                 GL_FLOAT,
                 NULL );

   // setting minifier:
   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST );

   // setting magnifier:
   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST );

   // describe and attach the texture id for pos to currently bound g-buffer
   glFramebufferTexture2D( GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           this->_g_buffer.pos_tex_loc,
                           0 );

// normal texture for g-buffer:
   glBindTexture( GL_TEXTURE_2D,
                  this->_g_buffer.nor_tex_loc );

   glTexImage2D( GL_TEXTURE_2D,
                 0,
                 GL_RGB16,
                 width,
                 height,
                 0,
                 GL_RGB,
                 GL_FLOAT,
                 NULL );

   // setting minifier:
   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST );

   // setting magnifier:
   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST );

   // describe and attatch the texture id for norm to Currently bound g-buffer
   glFramebufferTexture2D( GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT1,
                           GL_TEXTURE_2D,
                           this->_g_buffer.nor_tex_loc,
                           0 );

// specularity (specularity color + specularity intensity) for g-buffer:
   glBindTexture( GL_TEXTURE_2D,
                  this->_g_buffer.spe_tex_loc );

   glTexImage2D( GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 width,
                 height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 NULL );

   // sitting minifier:
   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST );

   // setting magnifier:
   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST );

   // attach the texture id for spec to Currently bound g-buffer
   glFramebufferTexture2D( GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT2,
                           GL_TEXTURE_2D,
                           this->_g_buffer.spe_tex_loc,
                           0 );
   
// albedo (RGBA) color texture for g-buffer
   glBindTexture( GL_TEXTURE_2D,
                  this->_g_buffer.alb_tex_loc );

   glTexImage2D( GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 width,
                 height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 NULL );

   // setting minifier:
   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST );

   // setting magnifier:
   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST );

   // attach the texture id to currently bound g-buffer
   glFramebufferTexture2D( GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT3,
                           GL_TEXTURE_2D,
                           this->_g_buffer.alb_tex_loc,
                           0 );

// Describe for fragment shader to write to these buffers (?)
   GLuint attachments[4] = { GL_COLOR_ATTACHMENT0,
                             GL_COLOR_ATTACHMENT1,
                             GL_COLOR_ATTACHMENT2,
                             GL_COLOR_ATTACHMENT3 };

   glDrawBuffers( 4, attachments );

   // Create a render buffer object for depth buffer
   GLuint depth_render_buffer_obj;

   glGenRenderbuffers( 1, &depth_render_buffer_obj );

   glBindRenderbuffer( GL_RENDERBUFFER, depth_render_buffer_obj );

   glRenderbufferStorage( GL_RENDERBUFFER,
                          GL_DEPTH_COMPONENT,
                          width,
                          height );

   // control the status of the frame buffer
   if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
      assert( false && "G-buffer status is not complete. " );

   // Bind to default buffer
   glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}
