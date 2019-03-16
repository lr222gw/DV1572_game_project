#include "Viewport.h"
#include "debug.h"

// TODO: l�s ViewPort -> Viewport

Viewport::Viewport( Vec3                      position,
                    GLFWwindow               *window,
                    SharedPtr<ShaderProgram>  shader_program,
                    Float32                   fov_rad )
:
   _fov            ( fov_rad          ),
   _aspect         ( -1.0f            ),
   _window         ( window           ),
   _shader_program ( shader_program   ),
   forward         ( 0.0f, 0.0f, 1.0f )
{
   _view = Transform( position );

   //TODO: Kan denna vara här?
   //Ligger här för att vi nu måste uppdatera vår viewports transforms view med LookAt...
   _view.look_at(this->forward, _view.get_position() + this->forward); // rotate view
   this->set_view(_view);

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

   //TODO: Kan denna vara här?
   //Ligger här för att vi nu måste uppdatera vår viewports transforms view med LookAt...
   _view.look_at(this->forward, _view.get_position() + this->forward); // rotate view
   this->set_view(_view);
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

void Viewport::bind_shader_program( SharedPtr<ShaderProgram> shader_program ) {
   _shader_program = shader_program;
   _generate_perspective();
}

void Viewport::update() {
   debug::view_mat4( _view.matrix, "viewport._view" );
   _update_aspect_ratio();
   _write_to_buffer();
}

void Viewport::_write_to_buffer() {
   //TODO: Vad händer här? ingen "use" funktion på shaderProgram, location hämtas men används ej
   _shader_program->use();
   auto location = _shader_program->get_location();



   //glm::vec3 scale;
   //glm::quat rotation;
   //glm::vec3 translation;
   //glm::vec3 skew;
   //glm::vec4 perspective;
   //
   //glm::decompose(_view.matrix, scale, rotation, translation, skew, perspective);
   //Mat4 rot = glm::toMat4(rotation) * glm::scale(Mat4(1.0f), scale) * glm::translate(Mat4(1.0f) , translation) ;
   //Mat4 rot3 = glm::toMat4(rotation) * glm::translate(Mat4(1.0f) , translation);
   //Mat4 rot4 = _view.get_rotation() * glm::translate(Mat4(1.0f) , translation);
   //Mat4 rot5 =  glm::translate(Mat4(1.0f) , translation) * _view.get_rotation();
   //Mat4 rot6 =  glm::translate(Mat4(1.0f) , translation) * glm::toMat4(rotation);
   //Mat4 rot2 = glm::toMat4(rotation);

   //Mat4 rotAlone = _view.get_rotation();
   //Mat4 posAlone = glm::translate(Mat4(1.0f), _view.get_position());
   //Mat4 scaleAlone = glm::scale(Mat4(1.0f), _view.get_scale());
   //Mat4 rot  = rotAlone *  posAlone * scaleAlone;

   //Mat4 og = _view.matrix;
   //Mat4 rot2 = posAlone *  rotAlone;

   //Mat4 rot2 = posAlone * rotAlone * scaleAlone;

   /*Mat4 fixed = glm::lookAt(_view.get_position(), _view.get_position() + this->forward, Vec3(0.0f, 1.0f, 0.0f));
   glUniformMatrix4fv( glGetUniformLocation(location, "view"),       1, GL_FALSE, &(fixed[0][0]) ); //*/
   //TODO: avgör om detta är rätt ställe att invertera viewmatrisen..
   glUniformMatrix4fv( glGetUniformLocation(location, "view"),       1, GL_FALSE, &(_view.matrix[0][0]) );

   //glUniformMatrix4fv( glGetUniformLocation(location, "view"),       1, GL_FALSE, &(_view.matrix[0][0]) );
   glUniformMatrix4fv( glGetUniformLocation(location, "projection"), 1, GL_FALSE, &(_projection[0][0]) );
}

// TODO: extract magnifier and minifier behavior to be changed in options
void Viewport::_g_buffer_init() {
   // @TAG{TEXTURE_CHANNEL}

  static bool initialized = false;
   // specular  =  specular RGB (=xyz) + specular intensity (=w)
   // albedo    =  albedo RGBA(= xyzw)
  if ( !initialized ) {
      //g-buffer init:
      glGenFramebuffers(  1, &(_g_buffer.buffer_loc) );
      // g-buffer Texture attatchments init:
      glGenTextures(      1, &(_g_buffer.alb_tex_loc) );
      glGenTextures(      1, &(_g_buffer.spe_tex_loc) );
      glGenTextures(      1, &(_g_buffer.nor_tex_loc) );
      glGenTextures(      1, &(_g_buffer.emi_tex_loc) );
      glGenTextures(      1, &(_g_buffer.dis_tex_loc) );
      glGenTextures(      1, &(_g_buffer.pos_tex_loc) );
	   glGenTextures(      1, &(_g_buffer.pic_tex_loc) );
      glGenRenderbuffers( 1, &(_g_buffer.depth_loc)   );
      initialized = true;
  }

   glBindFramebuffer( GL_FRAMEBUFFER,
                      _g_buffer.buffer_loc );



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
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           _g_buffer.alb_tex_loc,
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
                           GL_COLOR_ATTACHMENT1,
                           GL_TEXTURE_2D,
                           _g_buffer.spe_tex_loc,
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
                           GL_COLOR_ATTACHMENT2,
                           GL_TEXTURE_2D,
                           _g_buffer.nor_tex_loc,
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
                           GL_COLOR_ATTACHMENT3,
                           GL_TEXTURE_2D,
                           _g_buffer.emi_tex_loc,
                           0 );




   // displacement texture for g-buffer:
   glBindTexture( GL_TEXTURE_2D,
                  _g_buffer.dis_tex_loc );

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
                           GL_COLOR_ATTACHMENT4,
                           GL_TEXTURE_2D,
                           _g_buffer.dis_tex_loc,
                           0 );




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
                           GL_COLOR_ATTACHMENT5,
                           GL_TEXTURE_2D,
                           _g_buffer.pos_tex_loc,
                           0 );



// picking (RGBA) light texture for g-buffer

//   glBindTexture( GL_TEXTURE_2D,
//	               _g_buffer.pic_tex_loc);
//
//
//   glTexImage2D( GL_TEXTURE_2D,
//	              0,
//	              GL_RGBA8,
//	              width,
//	              height,
//	              0,
//	              GL_RGBA,
//	              GL_UNSIGNED_BYTE,
//	              NULL );

   glBindTexture( GL_TEXTURE_2D,
                  _g_buffer.pic_tex_loc );

   glTexImage2D( GL_TEXTURE_2D,
                 0,
                 GL_RGBA32F,
                 width,
                 height,
                 0,
                 GL_RGBA,
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
                            GL_COLOR_ATTACHMENT4,
                            GL_COLOR_ATTACHMENT5,
                            GL_COLOR_ATTACHMENT6 };

   glDrawBuffers( 7, attachments ); // TODO: BJÖRN SEE HÄR TILL MIGSJÄLV

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
