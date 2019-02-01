#include "Viewport.h"
// TODO: läs ViewPort -> Viewport

Viewport::Viewport(Vec3 position = { 0,0,0 }, Float32 fov = config::fov_rad) :
   _fov(fov)
{
   // TODO: bind _camera och uniform buffer för Mat4

   _model = Mat4(1.0f);
   _view = glm::lookAt(position, position + Vec3{ 0, 0, 1 }, Vec3{ 0,1,0 });
   _projection = glm::perspective(fov,
      config::aspect_ratio,
      config::near_plane,
      config::far_plane);

   _write_to_buffer();

}

void Viewport::transform(Mat4 transformation) {
   _model *= transformation;
   _write_to_buffer();
}

void Viewport::bind_shader_program(ShaderProgram &shapro) {

   _location = glGetUniformLocation(shapro.getProgramLoc(), "view_tranform");
   _write_to_buffer();
}


void Viewport::_write_to_buffer() {

   _camera = _projection * _view * _model;
   glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(_camera));
}