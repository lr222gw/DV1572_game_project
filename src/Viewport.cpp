#include "Viewport.h"
// TODO: lï¿½s ViewPort -> Viewport

Viewport::Viewport(Vec3 position , Vec3 rotation, Float32 fov) :
   _fov(fov)
{
   // TODO: bind _camera och uniform buffer fï¿½r Mat4

   _model = Mat4(1.0f);

   // validera
   _view = glm::lookAt( position,
                        Vec3{ 0, 0,  0 },
                        Vec3{ 0, 1,  0 } );

   // validera
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
   _model *= transformation; // validera
   _write_to_buffer();
}

void Viewport::bind_shader_program(ShaderProgram &shapro) {
   _location = glGetUniformLocation(shapro.getProgramLoc(), "mvp_transform");
   _write_to_buffer();
}

void Viewport::_update_model_matrix() {
   // validera?
   Mat4 new_position       = Mat4(1.0f);
        new_position[0][3] = _position.x;
        new_position[1][3] = _position.y;
        new_position[2][3] = _position.z;

   glm::quat rotation_quaternion {Vec4( _rotation, 0.0f) };

   _model = new_position * glm::toMat4(rotation_quaternion);;

   _write_to_buffer();
}


void Viewport::_write_to_buffer() {
   _camera = _projection * _view * _model; // validera?
   glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(_camera));
}