#pragma once

#include "misc/defs.h"

class Transform {
public:
   Transform( Vec3 const &position = Vec3(0.0f) ):
      _position ( position     ),
      _rotation ( Mat4( 1.0f ) ),
      _scale    ( Vec3( 1.0f ) ),
      matrix    ( _matrix      )
   {
      _update_matrix();
   }


   explicit Transform( Vec3 const &position,
                       Vec3 const &euler_angles,
                       Vec3 const &scale = Vec3( 1.0f ) )
   :
      _position ( position ),
      _scale    ( scale    ),
      matrix    ( _matrix  )
   {
      set_rotation( euler_angles );
   }


   explicit Transform( Vec3 const &position,
                       Mat4 const &rotation_matrix,
                       Vec3 const &scale = Vec3( 1.0f ) )
   :
      _position ( position ),
      _scale    ( scale    ),
      matrix    ( _matrix  )
   {
      set_rotation( rotation_matrix );
   }


   explicit Transform( Vec3 const &position,
                       Float32     angle_rad,
                       Vec3 const &axis,
                       Vec3 const &scale = Vec3( 1.0f ) )
   :
      _position ( position ),
      _scale    ( scale    ),
      matrix    ( _matrix  )
   {
      set_rotation( axis, angle_rad );
   }


   Transform( Transform const &other ):
      _position ( other._position ),
      _rotation ( other._rotation ),
      _scale    ( other._scale    ),
      _matrix   ( other._matrix   ),
      matrix    ( _matrix         )
   {
      _update_matrix();
   }

   Transform( Transform &&other ):
      _position ( std::move(other._position) ),
      _rotation ( std::move(other._rotation) ),
      _scale    ( std::move(other._scale)    ),
      _matrix   ( std::move(other._matrix)   ),
      matrix    ( _matrix                    )
   {}

   ~Transform() {}

   Mat4 get_transform() const;

   Transform &operator=( Transform const &other );

   Transform operator*( Transform const &right_hand_side ) const;
   void operator*=(     Transform const &right_hand_side );

   void set_position( Vec3 const &euler_angles );
   void set_rotation( Mat4 const &rotation_matrix );
   void set_rotation( Vec3 const &axis, Float32 angle_rad );
   void set_rotation( Vec3 const &rotation );
   void set_scale(    Vec3 const &scale );

   void rotate(     Vec3 const &euler_angles );
   void rotate(     Mat4 const &rotation_matrix );
   void rotate(     Vec3 const &axis, Float32 angle_rad );
   void rotate_deg( Vec3 const &axis, Float32 angle_deg);
   void look_at(    Vec3 const &position, Vec3 const up={0,1,0} );

   void scale(     Vec3 const &scale );
   void translate( Vec3 const &offset);

   // creates Transforms that exclusively translates, rotates, or scales
   // basically alternative constructors
   static Transform make_translation( Vec3 const &offset );
   static Transform make_rotation(    Vec3 const &euler_angles );
   static Transform make_rotation(    Mat4 const &rotation_matrix );
   static Transform make_rotation(    Vec3 const &axis, Float32 angle_rad );
   static Transform make_scale(       Vec3 const &scale );

   Vec3 get_position() const;
   Mat4 get_rotation() const;
   Vec3 get_scale()    const;

private:
   void _update_matrix();
   void _extract_data( Mat4 const &transformation_matrix );

   Vec3   _position;
   Mat4   _rotation;
   Vec3   _scale;
   Mat4   _matrix;

   static Mat4 const _identity_matrix;

public:
   Mat4 const &matrix;
};

// TODO: lerp, slerp


// Transform move_up = Transform::make_translation(...);
// myView.transform( rotation );
// 
// auto view = myView.get_view();
// view.translate(...);
// myView.set_view(view);