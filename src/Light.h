#pragma once

#include "defs.h"

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

enum LightType : Uint32 { point=0, spot=1, directional=2 };

struct LightData {
   LightType  type;
   Vec3  direction,
         position,
         color;
   Float32  intensity,
            radius,
            degree,
            specularity;
};


class Light {
public:
   Light() = delete;

   Light( SceneManager &scene, LightData light ):
      _id    ( _generate_id ),
      _scene ( scene        )
   {
      scene.add_light( _id, _light );
   }

   Light( Light &&other ) = delete;

   Light( Light const &other ):
      _id    ( other._id    ),
      _scene ( other._scene )
   {
      scene.add_light( _id, other.get_data() );
   }

   LightData get_data() const {
      return scene.get_data(_id);
   }

   void set_data( LightData const &data ) {
      scene.set_data( _id, data );
   }

   ~Light() {
      scene.remove_light( _id );
   }

private:
   Uint64 _generate_id() const {
      static next_id = 0;
      return next_id++;
   }

   Uint64        _id;
   SceneManager &_scene
};
