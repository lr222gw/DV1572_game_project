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

   Transform( Mat4 &&matrix ):
      _matrix( std::move(matrix) )
   {}

   Transform( Vec3 position,
              Mat4 rotation,
              Float32 scale )
   {
      // TODO: generera _matrix
   }

   ~Transform() {}

   Mat4 get_transform() const;

   Transform operator*( Transform const &right_hand_side ) const;
   void operator*=( Transform const &right_hand_side );

   void set_rotation(Vec3 const &rotation);
   void set_position(Vec3 const &position);
   void set_scale(Vec3 const &scale);

   void rotate(Quat const &rotation);
   void rotate(Vec3 const &, Float32);
   void rotate_deg(Vec3 const &axis, Float32 angle_deg);

   void look_at( Vec3 const &position, Vec3 const up={0,1,0} );

   void scale( Vec3 const &scale );
   void translate( Vec3 const &offset);

private:
   void _update_matrix();

   Mat4   _matrix;

   Vec3   _scale;
   Mat4   _rotation;
   Vec3   _position;
};
