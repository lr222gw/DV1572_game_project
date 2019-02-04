
#include "Transform.h"


Mat4 Transform::get_transform() const {
   return Mat4();
}

Transform& Transform::operator*( Transform const &right_hand_side ) const {
   return Transform( this->_matrix * right_hand_side._matrix );
}

void Transform::operator*=( Transform const &right_hand_side ) {
   this->_matrix *= right_hand_side._matrix;
}

void Transform::set_rotation(Vec3 const &rotation) {

}

void Transform::set_position(Vec3 const &rotation) {

}

void Transform::set_scale(Float32 scale) {

}

void Transform::rotate(Quat const &rotation) {
}

void Transform::rotate(Vec3 const &axis, Float32 angle_rad) {
   _matrix = glm::rotate(_matrix, angle_rad, axis);
}

void Transform::rotate_deg(Vec3 const &axis, Float32 angle_deg) {
   _matrix = glm::rotate(_matrix, glm::radians(angle_deg), axis);
}

void Transform::scale(Float32 scale) {
}

void Transform::translate(Vec3 const &offset) {
}

void Transform::look_at(Vec3 const &position, Vec3 const up) {
   Vec3 my_position { _matrix[3][0] ,
                      _matrix[3][1] , 
                      _matrix[3][2] };
   _matrix = glm::lookAt( my_position, position, up );
}
