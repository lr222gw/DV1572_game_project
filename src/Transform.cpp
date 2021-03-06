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

// void Transform::rotate_around( Vec3 position, Vec3 const &euler_angles ) {
//    auto relative_rotation   = _rotation * glm::toMat4( Quat( Vec4(euler_angles, 1.0f) ) );
//    relative_rotation[3][0] -= position.x;
//    relative_rotation[3][1] -= position.y;
//    relative_rotation[3][2] -= position.z;
//    _position *= relative_rotation;
//    relative_rotation[3][0] -= .0f;
//    relative_rotation[3][1] -= .0f;
//    relative_rotation[3][2] -= .0f;
//    _rotation = relative_rotation;
//    _update_matrix();
// }

// void Transform::rotate_around( Vec3 position, Mat4 const &rotation_matrix ) {
//    auto relative_rotation   = _rotation * rotation_matrix;
//    relative_rotation[3][0] -= position.x;
//    relative_rotation[3][1] -= position.y;
//    relative_rotation[3][2] -= position.z;
//    _position *= relative_rotation;
//    relative_rotation[3][0] -= .0f;
//    relative_rotation[3][1] -= .0f;
//    relative_rotation[3][2] -= .0f;
//    _rotation = relative_rotation;
//    _update_matrix();
// }

// void Transform::rotate_around( Vec3 position, Vec3 const &axis, Float32 angle_rad ) {
//    auto relative_rotation   = _rotation * glm::rotate( _rotation, angle_rad, axis );
//    relative_rotation[3][0] -= position.x;
//    relative_rotation[3][1] -= position.y;
//    relative_rotation[3][2] -= position.z;
//    _position *= relative_rotation;
//    relative_rotation[3][0] -= .0f;
//    relative_rotation[3][1] -= .0f;
//    relative_rotation[3][2] -= .0f;
//    _rotation = relative_rotation;
//    _update_matrix();
// }

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


//void Transform::look_at( Vec3 target_position, Vec3 up_direction={.0f, 1.0f, .0f} ) {
//   _rotation = glm::lookAt( _position, target_position, up_direction );
//   _update_matrix();
//}

void Transform::look_at(Vec3 const &forward, Vec3 pos, Vec3 const up ) {

   _rotation = glm::lookAt(Vec3(0.0f,0.0f,0.0f),forward, up );


   //Mat4 rotAlone = _rotation;
   //Mat4 posAlone = glm::translate(Mat4(1.0f),_position);
   //Mat4 scaleAlone = glm::scale(Mat4(1.0f), _scale);
   //Mat4 rot = rotAlone * posAlone * scaleAlone;

   //Mat4 og = _matrix;
   //Mat4 rot2 = posAlone *  rotAlone;

   //Mat4 rot2 = posAlone * rotAlone * scaleAlone;
   //Mat4 fixed = glm::lookAt(_position, _position + forward, Vec3(0.0f, 1.0f, 0.0f));

   Vec3 xAx, yAx, zAx;
   zAx = glm::normalize((_position + forward) - _position);
   xAx = glm::normalize(glm::cross(up, zAx));
   yAx = glm::cross(zAx, xAx);

   Mat4 sameAsFixed;

   //sameAsFixed[0][0] = xAx.x;                     sameAsFixed[0][1] = yAx.x;                      sameAsFixed[0][2] = zAx.x;                      sameAsFixed[0][3] = 0;
   //sameAsFixed[1][0] = xAx.y;                     sameAsFixed[1][1] = yAx.y;                      sameAsFixed[1][2] = zAx.y;                      sameAsFixed[1][3] = 0;
   //sameAsFixed[2][0] = xAx.z;                     sameAsFixed[2][1] = yAx.z;                      sameAsFixed[2][2] = zAx.z;                      sameAsFixed[2][3] = 0;
   //sameAsFixed[3][0] = glm::dot(xAx, -_position); sameAsFixed[3][1] = glm::dot(yAx, -_position);  sameAsFixed[3][2] = glm::dot(zAx, -_position);  sameAsFixed[3][3] = 1;
   ////sameAsFixed[3][0] = 0; sameAsFixed[3][1] = 0;  sameAsFixed[3][2] = 0;  sameAsFixed[3][3] = 1;


   //sameAsFixed[0][0] = -xAx.x;                     sameAsFixed[0][1] = -yAx.x;                      sameAsFixed[0][2] = -zAx.x;                      sameAsFixed[0][3] = 0;
   //sameAsFixed[1][0] = -xAx.y;                     sameAsFixed[1][1] = -yAx.y;                      sameAsFixed[1][2] = -zAx.y;                      sameAsFixed[1][3] = 0;
   //sameAsFixed[2][0] = xAx.z;                     sameAsFixed[2][1] = yAx.z;                      sameAsFixed[2][2] = zAx.z;                      sameAsFixed[2][3] = 0;
   //sameAsFixed[3][0] = glm::dot(xAx, _position); sameAsFixed[3][1] = glm::dot(yAx, _position);  sameAsFixed[3][2] = glm::dot(zAx, _position);  sameAsFixed[3][3] = 1;
   ////sameAsFixed[3][0] = 0; sameAsFixed[3][1] = 0;  sameAsFixed[3][2] = 0;  sameAsFixed[3][3] = 1;

   //Inverted values based on "Fixed"
   //sameAsFixed[0][0] = -xAx.x;                     sameAsFixed[0][1] = yAx.x;                      sameAsFixed[0][2] = -zAx.x;                      sameAsFixed[0][3] = 0;
   //sameAsFixed[1][0] = -xAx.y;                     sameAsFixed[1][1] = yAx.y;                      sameAsFixed[1][2] = -zAx.y;                      sameAsFixed[1][3] = 0;
   //sameAsFixed[2][0] = -xAx.z;                     sameAsFixed[2][1] = yAx.z;                      sameAsFixed[2][2] = -zAx.z;                      sameAsFixed[2][3] = 0;
   //sameAsFixed[3][0] = -glm::dot(xAx, -_position); sameAsFixed[3][1] = glm::dot(yAx, -_position);  sameAsFixed[3][2] = -glm::dot(zAx, -_position);  sameAsFixed[3][3] = 1;
   ////sameAsFixed[3][0] = 1; sameAsFixed[3][1] = 1;  sameAsFixed[3][2] = 1;  sameAsFixed[3][3] = 1;


   //Double inverted...
   sameAsFixed[0][0] = -xAx.x;                     sameAsFixed[0][1] = yAx.x;                      sameAsFixed[0][2] = -zAx.x;                      sameAsFixed[0][3] = 0;
   sameAsFixed[1][0] = -xAx.y;                     sameAsFixed[1][1] = yAx.y;                      sameAsFixed[1][2] = -zAx.y;                      sameAsFixed[1][3] = 0;
   sameAsFixed[2][0] = -xAx.z;                     sameAsFixed[2][1] = yAx.z;                      sameAsFixed[2][2] = -zAx.z;                      sameAsFixed[2][3] = 0;
   sameAsFixed[3][0] = -glm::dot(xAx, -_position); sameAsFixed[3][1] = glm::dot(yAx, -_position);  sameAsFixed[3][2] = -glm::dot(zAx, -_position);  sameAsFixed[3][3] = 1;


   //Mat4 donemat = _matrix;
   //Vec3 posInvert = _position;
   Vec3 oldPos = _position;
   _position = Vec3(0.0f,0.0f,0.0f);// posInvert;


   //sameAsFixed = glm::translate(Mat4(1.0f), posInvert) * sameAsFixed;
   _rotation =  sameAsFixed; //glm::translate(Mat4(1.0f), _position) *

   //_rotation = posAlone * sameAsFixed;
   //_matrix = sameAsFixed;

   _update_matrix();
   _position = oldPos;
   //int f = 3;



   //sameAsFixed = posAlone * sameAsFixed;

   //Mat4 Fel = glm::translate(_identity_matrix, _position)  // 3
   //   * _rotation                                      // 2
   //   * glm::scale(_identity_matrix, _scale); // 1

   //_update_matrix();




   //_rotation = glm::lookAt(pos, pos +forward, up );
   //_rotation = glm::lookAt(_position, forward, up );
   //_rotation = glm::lookAt(_position, _position + forward, up );

   //glm::vec3 scale;
   //glm::quat rotation;
   //glm::vec3 translation;
   //glm::vec3 skew;
   //glm::vec4 perspective;
   //glm::decompose(_rotation, scale, rotation, translation, skew, perspective);
   //_rotation = glm::toMat4(rotation);

   //this->_position = translation;
   //this->_matrix = _rotation;
   //_rotation = glm::toMat4(rotation);
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
   Vec3 euler_angles = Vec3(1); // TODO: implement. this is just so it is initialized so that the program runs

   //euler_angles.x = /*...*/; // bryt ut fr??n _rotations[0].xyz;
   //euler_angles.y = /*...*/; // bryt ut fr??n _rotations[1].xyz;
   //euler_angles.z = /*...*/; // bryt ut fr??n _rotations[2].xyz;

   // a d g 0    a,b,c = x axeln
   // b e h 0    d,e,f = y axeln
   // c f j 0    g,h,j = z axeln
   // 0 0 0 1

   // glm::vec3 scale;
   // glm::quat rotation;
   // glm::vec3 translation;
   // glm::vec3 skew;
   // glm::vec4 perspective;
   // glm::decompose(_rotation, scale, rotation, translation, skew, perspective);

   return euler_angles;
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

   //TODO: look up GLM::decompose

   _update_matrix();
}



void Transform::_update_matrix() {
   // omvandlar skala och position till mat4 transformer
   // och uppdaterar sedan klasstransformen till en kombination
   // av dessa och rotationsmatrisen
   _matrix = glm::translate( _identity_matrix, _position )
           * _rotation
           * glm::scale(     _identity_matrix, _scale    );

   // handle W // _matrix[3][3]
   if ( _matrix[3][3] != 1.0f )
      _matrix /= _matrix[3][3];
}

// void Transform::_update_matrix( Mat4 relative_rotation, Vec3 rotation_center_position={.0f, .0f, .0f} ) {
//    // omvandlar skala och position till mat4 transformer
//    // och uppdaterar sedan klasstransformen till en kombination
//    // av dessa och rotationsmatrisen
//    _matrix = glm::translate( _identity_matrix, _position );
//            * relative_rotation
//            * glm::translate( _identity_matrix, rotation_center_position )
//            * _rotation
//            * glm::scale(     _identity_matrix, _scale    );
//
//    // handle W // _matrix[3][3]
//    if ( _matrix[3][3] != 1.0f )
//       _matrix /= _matrix[3][3];
// }
