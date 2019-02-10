#pragma once

#include "misc/defs.h"
#include "Config.h"
#include "ShaderProgram.h"
#include "Transform.h"

struct GBufferData {
   GLuint  buffer_loc;
   GLuint  pos_tex_loc;
   GLuint  nor_tex_loc;
   GLuint  spe_tex_loc;
   GLuint  alb_tex_loc;
   GLuint  depth_loc;
};

// TODO: (låg prioritet) lägg till en  void make_active(); som i princip bara kallar på _write_to_buffer();
// TODO: fundera över multi-views och hur man ska hantera det
// TODO: uppåt vektorn baserat på normalen av ytan spelaren står på
class Viewport {
/*--------------- class member functions & operators ------------*/
public:
   Viewport( Vec3 position, GLFWwindow *window, Float32 fov = Config::fov_rad );
   Viewport( Viewport  const &)            = delete;
   Viewport( Viewport && )                 = delete;
   Viewport& operator=( Viewport const & ) = delete;
   Viewport& operator=( Viewport && )      = delete;

   void               transform( Transform const & );
   void               set_view(  Transform const & );
   Transform          get_view()     const;
   GBufferData const& get_g_buffer() const;
   void               bind_shader_program( ShaderProgram & );
   void               set_fov( Float32 fov_in_radians );
   void               update();
   void _g_buffer_init(); // TODO: make private again

private:
   void _write_to_buffer();
   void _update_aspect_ratio();
   void _generate_perspective();

/*--------------- class member variables & constants ------------*/
private:
   // TODO: (low priority) -- use Transform for projection as well?
   GBufferData _g_buffer;
   Float32     _fov;
   Float32     _aspect;
   GLint       _width;
   GLint       _height;
   Transform   _view;
   Mat4        _projection;
   GLFWwindow *_window;
   GLuint      _location;

public:
   Vec3  forward; // TODO: refactor away
   GLint const &width  = _width;
   GLint const &height = _height;
};
