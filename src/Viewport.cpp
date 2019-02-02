#include "Viewport.h"
// TODO: läs ViewPort -> Viewport

Viewport::Viewport(Vec3 position , Vec3 rotation, Float32 fov) :
   _fov(fov)
{
   // TODO: bind _camera och uniform buffer för Mat4

   _model = Mat4(1.0f);

   _view = glm::lookAt( position,
                        Vec3{ 0, 0,  0 },
                        Vec3{ 0, 1,  0 } );

   _projection = glm::perspective( fov,
                                   config::aspect_ratio,
                                   config::near_plane,
                                   config::far_plane);
   
   //tillfällig
   // _model = glm::rotate(_model, glm::radians(32.0f), Vec3(0.0f, 1.0f, 0.0f));
   
   _write_to_buffer();

}

void Viewport::set_position(Vec3 new_position) {
   _position = new_position;
   _update_view_matrix();
}

void Viewport::set_rotation(Vec3 new_rotation) {
   _rotation = new_rotation;
   _update_view_matrix();
}

void Viewport::transform(Mat4 transformation) {
   _model *= transformation;
   _write_to_buffer();
}

void Viewport::bind_shader_program(ShaderProgram &shapro) {
   _location = glGetUniformLocation(shapro.getProgramLoc(), "view_tranform");
   _write_to_buffer();
}

void Viewport::_update_view_matrix() {
   Mat4 new_position       = Mat4(1.0f);
        new_position[0][3] = _position.x;
        new_position[1][3] = _position.y;
        new_position[2][3] = _position.z;

   glm::quat rotation_quaternion { _rotation, 0.0f };

   _view = new_position * quaternion::toMat4(rotation_quaternion);;

   _write_to_buffer();
}


void Viewport::_write_to_buffer() {
   glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(_camera));
}