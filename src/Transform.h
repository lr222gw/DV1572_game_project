#pragma once

#include "misc/defs.h"

class Transform {
public:

   Mat4 get_transform();

   void make(Vec3 position, Mat4 rotation_matris, Float32 scale);
   void set_rotation(Vec3 rotation);
   void rotate(Quat rotation);
   void rotate(Vec3, Float32);
   void scale( Float32 scale);
   void translate( Vec3 offset);
   void look_at( Vec3 position);

private:
   Mat4 _matrix;

};