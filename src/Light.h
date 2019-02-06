#pragma once

#include "misc/defs.h"
#include "SceneManager.h"
/*
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
};*/


// TODO: use SceneManager pointer instead to allow assignment operator?

class Light {
public:
   Light() = delete;

   Light( SceneManager &scene, LightData light ):
      _id    ( _generate_id() ),
      _scene ( scene        )
   {
      scene.add_light( _id, light );
   }

   Light( Light &&other ) = delete;

   Light( Light const &other ):
      _id    ( other._id    ),
      _scene ( other._scene )
   {
      _scene.add_light( _id, other.get_data() );
   }

   ~Light() {
      _scene.remove_light( _id );
   }

   Light& operator=( Light const & ) = delete; // TODO?

   Light& operator=( Light && ) = delete;

   LightData get_data() const {
      return _scene.get_data(_id);
   }

   void set_data( LightData const &data ) {
      _scene.set_light( _id, data ); 
   }

   Uint64 get_id() const;

private:
   Uint64 _generate_id() const {
      static Uint64 next_id = 0;
      return next_id++;
   }

   Uint64        _id;
   SceneManager &_scene;
};
