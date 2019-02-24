#pragma once

#include "misc/defs.h"
#include "SceneManager.h"

/* // TODO: refactor back from SceneManager.h?
struct DirLight {
   Vec3  direction,
         color;
   Float32  intensity,
            specular;
};

struct SpotLight {
   Vec3  position,
         direction,
         color;
   Float32  intensity,
            radius,
            degree,
            specular;
};

// TODO: light falloff
// TODO: slå ihop ljus?

struct PointLight {
   Vec3  position,
         color;
   Float32  intensity,
            radius,
            specular;
};
*/

// TODO: use SceneManager pointer instead to allow assignment operator?

class Light {
public:
   Light()           = delete;
   Light( Light && ) = delete;

   Light( SceneManager &scene, LightData light ):
      _id    ( _generate_id() ),
      _scene ( scene        )
   {
      scene.add_light( _id, light );
   }

   Light( Light const &other ):
      _id    ( other._id    ),
      _scene ( other._scene )
   {
      _scene.add_light( _id, other.get_data() );
   }

   ~Light() {
      _scene.remove_light( _id );
   }

   Light&     operator=( Light const  & ) = delete; // TODO: ?
   Light&     operator=( Light       && ) = delete;

   [[nodiscard]] LightData  get_data() const;
   void                     set_data( LightData const & );

   [[nodiscard]] Uint64     get_id() const;

private:
   [[nodiscard]] Uint64    _generate_id() const;

   //Mat4        _proj_view_matrix;
   Uint64        _id;
   SceneManager &_scene;
};
