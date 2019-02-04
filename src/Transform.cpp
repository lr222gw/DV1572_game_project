#include "Transform.h"

Mat4 const Transform::_identity_matrix ( 1.0f );

Transform &Transform::operator=( Transform const &other ) {
   if ( &other != this ) {
      this->_position = other._position;
      this->_rotation = other._rotation;
      this->_scale    = other._scale;
      this->_matrix   = other._matrix;
   }
   return *this;
}

Mat4 Transform::get_transform() const {
   return _matrix;
}

Transform Transform::operator*( Transform const &right_hand_side ) const {
   return Transform( this->_position + right_hand_side._position,
                     this->_rotation * right_hand_side._rotation,
                     this->_scale    * right_hand_side._scale );
}

void Transform::operator*=( Transform const &right_hand_side ) {
   this->_position += right_hand_side._position,
   this->_rotation *= right_hand_side._rotation,
   this->_scale    *= right_hand_side._scale;
   _update_matrix();
}

void Transform::set_rotation(Vec3 const &rotation) {
   _rotation = rotation;
   _update_matrix();
}

void Transform::set_position(Vec3 const &position ) {
   _position = position;
   _update_matrix();
}

void Transform::set_scale(Vec3 const &scale) {
   _scale = scale;
   _update_matrix();
}

void Transform::rotate(Vec3 const &rotation) {
   // generera mat4 via quad
   _rotation += rotation; //?
   _update_matrix();
}

// void Transform::rotate(Vec3 const &axis, Float32 angle_rad) {
//    _rotation *= glm::rotate(_rotation, angle_rad, axis);
//    _update_matrix();
// }

// void Transform::rotate_deg(Vec3 const &axis, Float32 angle_deg) {
//    _rotation *= glm::rotate(_rotation, glm::radians(angle_deg), axis);
//    _update_matrix();
// }

void Transform::scale(Vec3 const &scale) {
   _scale *= scale;
   _update_matrix();
}

void Transform::translate(Vec3 const &offset) {
   _position += offset;
   _update_matrix();
}

// void Transform::look_at(Vec3 const &position, Vec3 const up) {
//    _rotation = glm::lookAt( _position, position, up );
//    _rotation[3][0] = 0;
//    _rotation[3][1] = 0; // TODO: verifiera att ordningen är rätt på matrisaccessen
//    _rotation[3][2] = 0;
//    _update_matrix();
// }

Transform Transform::make_translation( Vec3 const &offset ) {
   return Transform( offset );
}

Transform Transform::make_rotation( Vec3 const &rotations_rad ) {
   return Transform( Vec3(0.0f), rotations_rad ); // 1.0f?
}

Transform Transform::make_scale( Vec3 const &scales ) {
   return Transform( Vec3(0.0f), Vec3(1.0f), scales );
}

Vec3 Transform::get_position() const {
   return _position;
}

Vec3 Transform::get_rotation() const {
   return _rotation;
}

Vec3 Transform::get_scale() const {
   return _scale;
}



void Transform::_update_matrix() {
   // omvandlar skala och position till mat4 transformer
   // och uppdaterar sedan klasstransformen till en kombination
   // av dessa och rotationsmatrisen
   _matrix = glm::toMat4(Quat(Vec4(_rotation, 1.0f)))      // 3
           * glm::translate( _identity_matrix, _position ) // 2
           * glm::scale( _identity_matrix, _scale );       // 1
}
