#include "Viewport.h"
// TODO: lï¿½s ViewPort -> Viewport

Viewport::Viewport(Vec3 position, Float32 fov_rad) :
   _fov (fov_rad)
{
   // TODO: bind _camera och uniform buffer fï¿½r Mat4

   //_model = Mat4(1.0f);

   // validera
   _view = Mat4(1.0f);
   _view = glm::lookAt( position,
                        position + Vec3{ 0, 0, -1 },
                        Vec3{ 0, 1, 0 } );

   // validera
   _projection = Mat4(1.0f);
   _projection = glm::perspective( _fov,
                                   Float32(config::width) / Float32(config::height), // config::aspect_ratio,
                                   config::near_plane,
                                   config::far_plane);
   
   //tillfällig
   _model = Mat4( 1.0f );
   _model = glm::translate(_model, glm::vec3(0.0f, 0.0f, 0.0f)); // Translate it down a bit so it's at the center of the scene
   _model = glm::scale(_model, glm::vec3(0.1f, 0.1f, 0.1f));   // It's a bit too big for our scene, so scale it down
   
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
   _view *= transformation; // validera
   _write_to_buffer();
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

void Viewport::_update_view_matrix() {
   // _position = vec3 med x,y,z
   // _rotation = vec3 med rotation kring x, y, z i radianer
   // dvs.
   // _rotation.x = hur många radianer roterar vi kring X-axeln
   // _rotation.y = hur många radianer roterar vi kring Y-axeln
   // _rotation.z = hur många radianer roterar vi kring Z-axeln

 /*/ den här använder inte rotation
      _view = glm::lookAt( _position,
                           _position + Vec3{ 0, 0, -1 },
                           Vec3{ 0, 1, 0 } );
//*/

/// den här roterar med kvatern
   glm::quat rotation_quaternion {Vec4( _rotation, 0.0f) };

   _view       = glm::toMat4(rotation_quaternion);;
   _view[3][0] = _position.x;
   _view[3][1] = _position.y;
   _view[3][2] = _position.z;
//*/
   _write_to_buffer();
}


void Viewport::_write_to_buffer() {
   // _camera = _projection * _view; // * _model; // validera?
   // glUniformMatrix4fv( _location, 1, GL_FALSE, glm::value_ptr(_camera) );
   //glUniformMatrix4fv( glGetUniformLocation(_location, "model"),      1, GL_FALSE, glm::value_ptr(_model)      );
   glUniformMatrix4fv( glGetUniformLocation(_location, "model"),      1, GL_FALSE, &_model[0][0]      );
   //glUniformMatrix4fv( glGetUniformLocation(_location, "view"),       1, GL_FALSE, glm::value_ptr(_view)       );
   glUniformMatrix4fv( glGetUniformLocation(_location, "view"),       1, GL_FALSE, &_view[0][0]       );
   //glUniformMatrix4fv( glGetUniformLocation(_location, "projection"), 1, GL_FALSE, glm::value_ptr(_projection) );
   glUniformMatrix4fv( glGetUniformLocation(_location, "projection"), 1, GL_FALSE, &_projection[0][0] );
   
}
