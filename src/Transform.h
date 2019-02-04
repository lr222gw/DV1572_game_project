#pragma once

#include "misc/defs.h"

class Transform {
public:
   Transform():
      _matrix( 1.0f )
   {}

   Transform( Mat4 const &matrix ):
      _matrix( matrix )
   {}

   Transform( Vec3 position,
              Mat4 rotation,
              Float32 scale )
   {
      // TODO: generera _matrix
   }

   ~Transform() {}

   Mat4 get_transform() const;

   Transform& operator*( Transform const &right_hand_side ) const;
   void operator*=( Transform const &right_hand_side );

   void set_rotation(Vec3 const &rotation);
   void set_position(Vec3 const &rotation);
   void set_scale(Float32 scale);

   void rotate(Quat const &rotation);
   void rotate(Vec3 const &, Float32);
   void rotate_deg(Vec3 const &axis, Float32 angle_deg);

   void look_at( Vec3 const &position, Vec3 const up={0,1,0} );

   void scale( Float32 scale );
   void translate( Vec3 const &offset);

private:
   Mat4 _matrix;
};
