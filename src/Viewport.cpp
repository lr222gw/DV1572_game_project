#include "Viewport.h"
#include "debug.h"

// TODO: l�s ViewPort -> Viewport

Viewport::Viewport( Vec3 position, GLFWwindow *window, Float32 fov_rad ):
   _fov    ( fov_rad ),
   _aspect ( -1.0f   ),
   _window ( window  ),
   forward (0.0f,0.0f,1.0f)
{
   _view = Transform( position );
   _update_aspect_ratio();
   _write_to_buffer();
}

void Viewport::_update_aspect_ratio() {
   glfwGetWindowSize( _window, &_width, &_height );
   Float32 aspect = Float32(_width) / Float32(_height);
   if ( aspect != _aspect ) {
      _aspect = aspect;
      _g_buffer_init();
      _generate_perspective();
      glfwMakeContextCurrent( _window );
      glfwGetFramebufferSize( _window, &_width, &_height );
      glViewport( 0, 0, _width, _height );
   }
}

void Viewport::_generate_perspective() {
   _projection = glm::perspective( _fov,
                                   _aspect,
                                   Config::near_plane,
                                   Config::far_plane );
   _write_to_buffer();
}

void Viewport::transform( Transform const &transform ) {
   _view *= transform;
   _write_to_buffer();
}

void Viewport::set_view( Transform const &transform ) {
   _view = transform;
   _write_to_buffer();
}

[[nodiscard]] Transform Viewport::get_view() const {
   return _view;
}

[[nodiscard]] GBufferData const &Viewport::get_g_buffer() const {
   return _g_buffer;
}

void Viewport::set_fov( Float32 fov_rad ) {
   _fov = fov_rad;
   _generate_perspective();
}

void Viewport::bind_shader_program( ShaderProgram &shader_program ) {
   _location = shader_program.get_location();
   _generate_perspective();
   _write_to_buffer();
}

void Viewport::update() {
   debug::view_mat4( _view.matrix, "viewport._view" );
   _update_aspect_ratio();
   _write_to_buffer();

}

void Viewport::_write_to_buffer() {
   glUniformMatrix4fv( glGetUniformLocation(_location, "view"),       1, GL_FALSE, &(_view.matrix[0][0]) );
   glUniformMatrix4fv( glGetUniformLocation(_location, "projection"), 1, GL_FALSE, &(_projection[0][0]) );
}

// TODO: extract magnifier and minifier behavior to be changed in options
void Viewport::_g_buffer_init() {
  static bool initialized = false;
   // specular  =  specular RGB (=xyz) + specular intensity (=w)
   // albedo    =  albedo RGBA(= xyzw)
  if ( !initialized ) {
      //g-buffer init:
      glGenFramebuffers(  1, &(_g_buffer.buffer_loc) );
      // g-buffer Texture attatchments init:
      glGenTextures(      1, &(_g_buffer.pos_tex_loc) );
      glGenTextures(      1, &(_g_buffer.nor_tex_loc) );
      glGenTextures(      1, &(_g_buffer.spe_tex_loc) );
      glGenTextures(      1, &(_g_buffer.alb_tex_loc) );
      glGenTextures(      1, &(_g_buffer.emi_tex_loc) );
	   glGenTextures(      1, &(_g_buffer.pic_tex_loc) );
      //
      glGenRenderbuffers( 1, &(_g_buffer.depth_loc)   );
      initialized = true;
  }

   glBindFramebuffer( GL_FRAMEBUFFER,
                      _g_buffer.buffer_loc );



// position texture for g-buffer:
   glBindTexture( GL_TEXTURE_2D,
                  _g_buffer.pos_tex_loc );

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
                           _g_buffer.pos_tex_loc,
                           0 );



// normal texture for g-buffer:
   glBindTexture( GL_TEXTURE_2D,
                  _g_buffer.nor_tex_loc );

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
                           _g_buffer.nor_tex_loc,
                           0 );



// specularity (specularity color + specularity intensity) for g-buffer:
   glBindTexture( GL_TEXTURE_2D,
                  _g_buffer.spe_tex_loc );

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
                           _g_buffer.spe_tex_loc,
                           0 );



// albedo (RGBA) color texture for g-buffer
   glBindTexture( GL_TEXTURE_2D,
                  _g_buffer.alb_tex_loc );

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
                           _g_buffer.alb_tex_loc,
                           0 );



// emission (RGBA) light texture for g-buffer
   glBindTexture( GL_TEXTURE_2D,
                  _g_buffer.emi_tex_loc );

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
                           GL_COLOR_ATTACHMENT5,
                           GL_TEXTURE_2D,
                           _g_buffer.emi_tex_loc,
                           0 );

// mouse picking texture for g-buffer
   glBindTexture( GL_TEXTURE_2D,
                  _g_buffer.pic_tex_loc );

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
                    GL_NEAREST);

   // setting magnifier:
   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST);

   // attach the texture id to currently bound g-buffer
   glFramebufferTexture2D( GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT6,
                           GL_TEXTURE_2D,
                           _g_buffer.pic_tex_loc,
                           0 );




// Describe for fragment shader to write to these buffers (?)
   GLuint attachments[] = { GL_COLOR_ATTACHMENT0,
                            GL_COLOR_ATTACHMENT1,
                            GL_COLOR_ATTACHMENT2,
                            GL_COLOR_ATTACHMENT3,
                            GL_COLOR_ATTACHMENT5,
                            GL_COLOR_ATTACHMENT6 };

   glDrawBuffers( 6, attachments );

// Create a render buffer object for depth buffer

   glBindRenderbuffer( GL_RENDERBUFFER, _g_buffer.depth_loc );

   glRenderbufferStorage( GL_RENDERBUFFER,
                          GL_DEPTH_COMPONENT,
                          width,
                          height );

   // TODO: describe
   glFramebufferRenderbuffer( GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER,
                              _g_buffer.depth_loc );

   // control the status of the frame buffer
   if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
      assert( false && "G-buffer status is not complete. " );

   // Bind to default buffer
   glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}
