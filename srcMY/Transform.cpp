#include "Transform.h"
#include "debug.h"

Mat4 const Transform::_identity_matrix ( 1.0f );

Transform &Transform::operator=( Transform const &other ) {
   if ( &other != this ) {
      _position = other._position;
      _rotation = other._rotation;
      _scale    = other._scale;
      _matrix   = other._matrix;
   }
   return *this;
}

Transform &Transform::operator=( Transform &&other ) {
   // if ( &other != this ) {
      _position = std::move( other._position );
      _rotation = std::move( other._rotation );
      _scale    = std::move( other._scale    );
      _matrix   = std::move( other._matrix   );
   // }
   return *this;
}

Mat4 Transform::get_transform() const {
   return _matrix;
}

Transform Transform::operator*( Transform const &right_hand_side ) const {
   return Transform( _position + right_hand_side._position,
                     _rotation * right_hand_side._rotation,
                     _scale    * right_hand_side._scale );
}

void Transform::operator*=( Transform const &right_hand_side ) {
   _position += right_hand_side._position,
   _rotation *= right_hand_side._rotation,
   _scale    *= right_hand_side._scale;
   _update_matrix();
}

void Transform::set_rotation( Vec3 const &euler_angles ) {
   _rotation = glm::toMat4( Quat( Vec4(euler_angles, 1.0f) ) );
   _update_matrix();
}

void Transform::set_rotation( Mat4 const &rotation_matrix ) {
   // TODO: verify veracity of matrix
   _rotation = rotation_matrix;
   _update_matrix();
}

void Transform::set_rotation( Vec3 const &axis, Float32 angle_rad ) {
   _rotation = glm::rotate( _rotation, glm::radians(angle_rad), axis );
   _update_matrix();
}

void Transform::set_position( Vec3 const &position ) {
   _position = position;
   _update_matrix();
}

void Transform::set_scale( Vec3 const &scale ) {
   _scale = scale;
   _update_matrix();
}

void Transform::rotate(Vec3 const &euler_angles) {
   _rotation *= glm::toMat4( Quat( Vec4(euler_angles, 1.0f) ) );
   _update_matrix();
}

void Transform::rotate( Mat4 const &rotation_matrix ) {
   // TODO: verify veracity of matrix
   _rotation *= rotation_matrix;
   _update_matrix();
}

void Transform::rotate(Vec3 const &axis, Float32 angle_rad) {
   _rotation *= glm::rotate( _rotation, angle_rad, axis );
   _update_matrix();
}

void Transform::rotate_deg(Vec3 const &axis, Float32 angle_deg) {
   _rotation *= glm::rotate( _rotation, glm::radians(angle_deg), axis );
   _update_matrix();
}

void Transform::scale(Vec3 const &scale) {
   _scale *= scale;
   _update_matrix();
}

void Transform::translate(Vec3 const &offset) {
   _position += offset;
   _update_matrix();
}

void Transform::look_at(Vec3 const &forward, Vec3 const up) {
   _rotation = glm::lookAt( Vec3(0.0f), forward, up );
   _update_matrix();
}

Transform Transform::make_translation( Vec3 const &offset ) {
   Transform product;
   product.translate( offset );
   return product;
}

Transform Transform::make_rotation( Vec3 const &euler_angles ) {
   Transform product;
   product.rotate( euler_angles );
   return product;
}

Transform Transform::make_rotation( Mat4 const &rotation_matrix ) {
   // TODO: verify veracity of matrix
   Transform product;
   product.rotate( rotation_matrix );
   return product;
}

Transform Transform::make_rotation( Vec3 const &axis, Float32 angle_rad ) {
   Transform product;
   product.rotate( axis, angle_rad );
   return product;
}

Transform Transform::make_scale( Vec3 const &scales ) {
   return Transform( Vec3(0.0f), Vec3(1.0f), scales );
}

Vec3 Transform::get_position() const {
   return _position;
}

Mat4 Transform::get_rotation() const {
   return _rotation;
}

//Vec3 Transform::get_rotation_vec3() const
//{
//   Vec4 rotation_vector(1.0f,1.0,1.0,0.0);
//
//   Mat4 rotation_matrix = this->get_rotation();
//   //rotation_vector.x  = 
//
//   return rotationmatrix;
//}

Vec3 Transform::get_scale() const {
   return _scale;
}

Vec3 Transform::get_euler_angles() const {
   // TODO
   return Vec3( 0.0f );
}

void Transform::_extract_data( Mat4 const &mat ) {
   _position = Vec3( mat[3][0],
                     mat[3][1],
                     mat[3][2] );

   _scale = Vec3( glm::length( Vec3( mat[0][0],
                                     mat[0][1],
                                     mat[0][2] ) ),

                  glm::length( Vec3( mat[1][0],
                                     mat[1][1],
                                     mat[1][2] ) ),

                  glm::length( Vec3( mat[2][0],
                                     mat[2][1],
                                     mat[2][2] ) ) );

   Mat4 rot_mat ( 1.0f );
   rot_mat[0][0] = mat[0][0] / _scale[0];
   rot_mat[0][1] = mat[0][1] / _scale[0];
   rot_mat[0][2] = mat[0][2] / _scale[0];

   rot_mat[1][0] = mat[1][0] / _scale[1];
   rot_mat[1][1] = mat[1][1] / _scale[1];
   rot_mat[1][2] = mat[1][2] / _scale[1];

   rot_mat[2][0] = mat[2][0] / _scale[2];
   rot_mat[2][1] = mat[2][1] / _scale[2];
   rot_mat[2][2] = mat[2][2] / _scale[2];

  // _rotation = ... // TODO: bryt ut vec3 från mat4

   _update_matrix();
}



void Transform::_update_matrix() {
   // omvandlar skala och position till mat4 transformer
   // och uppdaterar sedan klasstransformen till en kombination
   // av dessa och rotationsmatrisen
   _matrix = _rotation                                      // 3
           * glm::translate( _identity_matrix, _position )  // 2
           * glm::scale(     _identity_matrix, _scale    ); // 1

   // handle W // _matrix[3][3]
   if ( _matrix[3][3] != 1.0f )
      _matrix /= _matrix[3][3];
}
