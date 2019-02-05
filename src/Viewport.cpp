#include "Viewport.h"
#include "debug.h"

// TODO: lï¿½s ViewPort -> Viewport

Viewport::Viewport(Vec3 position, GLFWwindow &window, Float32 fov_rad):
   _fov    ( fov_rad ),
   _window ( window  ),
   front(1.0f,1.0f,1.0f)
{
   // TODO: bind _camera och uniform buffer fï¿½r Mat4

   //_model = Mat4(1.0f);

   // validera
   _view = Transform( position );

   update(); // will generate _projection matrix

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
   // _rotation.x = hur mï¿½nga radianer roterar vi kring X-axeln
   // _rotation.y = hur mï¿½nga radianer roterar vi kring Y-axeln
   // _rotation.z = hur mï¿½nga radianer roterar vi kring Z-axeln

 /*/ den hï¿½r anvï¿½nder inte rotation
      _view = glm::lookAt( _position,
                           _position + Vec3{ 0, 0, -1 },
                           Vec3{ 0, 1, 0 } );
//*/

/// den hï¿½r roterar med kvatern
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

   Int32 width, height;
   glfwGetWindowSize( &_window, width, height );
   Float32  aspect = Float32(width) / Float32(height);

   // TODO: gör så att projection bara uppdateras när aspect ration ändras
   _projection = glm::perspective( _fov,
                                   aspect,
                                   config::near_plane,
                                   config::far_plane );
   _write_to_buffer();
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
