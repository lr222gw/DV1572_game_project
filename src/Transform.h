#pragma once

#include "misc/defs.h"

class Transform {
public:
   Mat4 const &matrix;

   Transform( Vec3 position     = Vec3( 0.0f ),
              Vec3 euler_angles = Vec3( 0.0f ),
              Vec3    scale     = Vec3( 1.0f ) )
   :
      _position ( position ),
      _scale    ( scale    ),
      matrix    ( _matrix  )
   {
      set_rotation( euler_angles );
      _update_matrix();
   }

   Transform( Vec3 position        = Vec3( 0.0f ),
              Mat4 rotation_matrix = Mat4( 1.0f ),
              Vec3    scale        = Vec3( 1.0f ) )
   :
      _position ( position ),
      _scale    ( scale    ),
      matrix    ( _matrix  )
   {
      set_rotation( rotation_matrix );
      _update_matrix();
   }

   Transform( Vec3    position  = Vec3( 0.0f ),
              Float32 angle_rad = 0.0f,
              Vec3    axis      = Vec3( 0.0f ),
              Vec3    scale     = Vec3( 1.0f ) )
   :
      _position ( position ),
      _scale    ( scale    ),
      matrix    ( _matrix  )
   {
      set_rotation( axis, angle_rad );
      _update_matrix();
   }


   Transform( Mat4 transform_matrix )
   :
      matrix( _matrix )
   {
      _extract_data( transform_matrix );
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

   Transform( Transform && ) = delete;

   ~Transform() {}

   Mat4 get_transform() const;

   Transform &operator=( Transform const &other );

   Transform operator*( Transform const &right_hand_side ) const;
   void operator*=(     Transform const &right_hand_side );

   void set_position( Vec3 const &euler_angles );
   void set_rotation( Mat4 const &rotation_matrix );
   void set_rotation( Vec3 const &axis, Float32 deg_rad );
   void set_rotation( Vec3 const &rotation );
   void set_scale(    Vec3 const &scale );

   void rotate(     Vec3 const &euler_angles );
   void rotate(     Mat4 const &rotation_matrix );
   void rotate(     Vec3 const &axis, Float32 degree_rad );
   void rotate_deg( Vec3 const &axis, Float32 angle_deg);
   void look_at(    Vec3 const &position, Vec3 const up={0,1,0} );

   void scale(     Vec3 const &scale );
   void translate( Vec3 const &offset);

   // creates Transforms that exclusively translates, rotates, or scales
   // basically alternative constructors
   static Transform make_translation( Vec3 const &offset );
   static Transform make_rotation( Vec3 const &euler_angles );
   static Transform make_rotation( Mat4 const &rotation_matrix );
   static Transform make_rotation( Vec3 const &axis, Float32 deg_rad );
   static Transform make_scale( Vec3 const &scale );

   Vec3 get_position() const;
   Mat4 get_rotation() const;
   Vec3 get_scale()    const;

private:
   void _update_matrix();

   void _extract_data( Mat4 const &transformation_matrix );

   Mat4   _matrix;

   Vec3   _position;
   Mat4   _rotation;
   Vec3   _scale;

   static Mat4 const _identity_matrix;
};

// TODO: lerp, slerp


// Transform move_up = Transform::make_translation(...);
// myView.transform( rotation );
// 
// auto view = myView.get_view();
// view.translate(...);
// myView.set_view(view);