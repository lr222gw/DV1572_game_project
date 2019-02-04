
#include "Transform.h"


Mat4 Transform::get_transform() const {
   return _matrix;
}

Transform Transform::operator*( Transform const &right_hand_side ) const {
   return Transform( this->_matrix * right_hand_side._matrix );
}

void Transform::operator*=( Transform const &right_hand_side ) {
   this->_matrix *= right_hand_side._matrix;
}

void Transform::set_rotation(Vec3 const &rotation) {
   _rotation = glm::toMat4( Quat(Vec4(rotation, 1.0f)) );
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

void Transform::rotate(Quat const &rotation) {
   // generera mat4 via quad
   _rotation *= glm::toMat4(rotation);
   _update_matrix();
}

void Transform::rotate(Vec3 const &axis, Float32 angle_rad) {
   _rotation *= glm::rotate(_rotation, angle_rad, axis);
   _update_matrix();
}

void Transform::rotate_deg(Vec3 const &axis, Float32 angle_deg) {
   _rotation *= glm::rotate(_rotation, glm::radians(angle_deg), axis);
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

void Transform::look_at(Vec3 const &position, Vec3 const up) {
   _rotation = glm::lookAt( _position, position, up );
   _rotation[3][0] = 0;
   _rotation[3][1] = 0; // TODO: verifiera att ordningen är rätt på matrisaccessen
   _rotation[3][2] = 0;
   _update_matrix();
}

void Transform::_update_matrix() {
   // omvandlar skala och position till mat4 transformer
   // och uppdaterar sedan klasstransformen till en kombination
   // av dessa och rotationsmatrisen
   _matrix = _rotation                             // 3
           * glm::translate( _matrix, _position )  // 2
           * glm::scale( Mat4(1.0f), _scale );     // 1
}