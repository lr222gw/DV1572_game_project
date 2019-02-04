#pragma once

#include "misc/defs.h"

class Transform {
public:
   Transform():
      _matrix( 1.0f )
   {}

   Transform( Vec3 position,
              Mat4 rotation,
              Float32 scale )
   {
      // TODO: generera _matrix
   }

   ~Transform() {}

   Mat4 get_transform();

   Transform& operator*( Transform const &right_hand_side );
   void operator*=( Transform const &right_hand_side );

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