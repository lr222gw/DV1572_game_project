
#include "Transform.h"


Mat4 Transform::get_transform()
{
   return Mat4();
}

Transform& Transform::operator*( Transform const &right_hand_side ) {
   return this->_matrix * right_hand_side._matrix;
}

void operator*=( Transform const &right_hand_side ) {
   this->_matrix *= right_hand_side._matrix;
}

void Transform::make(Vec3 position, Mat4 rotation_matris, Float32 scale)
{

}

void Transform::set_rotation(Vec3 rotation)
{
}

void Transform::rotate(Quat rotation)
{
}

void Transform::rotate(Vec3 axis, Float32 angle_rad)
{
   _matrix = glm::rotate(_matrix, angle_rad, axis);
}

void Transform::rotate_deg(Vec3 axis, Float32 angle_deg)
{
   _matrix = glm::rotate(_matrix, glm::radians(angle_deg), axis);
}

void Transform::scale(Float32 scale)
{
}

void Transform::translate(Vec3 offset)
{
}

void Transform::look_at(Vec3 position)
{
}
