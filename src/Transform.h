#pragma once

#include "misc/defs.h"

class Transform {
public:
   Mat4 const &matrix;

   Transform( Vec3 position = Vec3( 0.0f ),
              Mat4 rotation = _identity_matrix,
              Vec3    scale = Vec3( 1.0f ) )
   :
      _position ( position ),
      _rotation ( rotation ),
      _scale    ( scale    ),
      matrix    ( _matrix  )

   {
      _update_matrix();
   }

   ~Transform() {}

   Mat4 get_transform() const;

   Transform &operator=( Transform const &other );

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

   // creates Transforms that exclusively translates, rotates, or scales
   static Transform make_translation( Vec3 const &offset );
   static Transform make_rotation( Vec3 const &scales );
   static Transform make_scale( Vec3 const &rotations_rad );

private:
   void _update_matrix();

   Mat4   _matrix;

   Vec3   _scale;
   Mat4   _rotation;
   Vec3   _position;

   static Mat4 const _identity_matrix ( 1.0f );
};

// TODO: lerp, slerp


// Transform move_up = Transform::make_translation(...);
// myView.transform( rotation );
// 
// auto view = myView.get_view();
// view.translate(...);
// myView.set_view(view);