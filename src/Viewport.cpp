#include "Viewport.h"
// TODO: läs ViewPort -> Viewport

Viewport::Viewport(Vec3 position , Vec3 rotation, Float32 fov) :
   _fov(fov)
{
   // TODO: bind _camera och uniform buffer för Mat4

   _model = Mat4(1.0f);
   _view = glm::lookAt(position, Vec3{ 0, 0, 1 }, Vec3{ 0,1,0 });
   _projection = glm::perspective(  fov,
                                    config::aspect_ratio,
                                    config::near_plane,
                                    config::far_plane);
   
   //tillfällig
   _model = glm::rotate(_model, glm::radians(32.0f), Vec3(0.0f, 1.0f, 0.0f));
   
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