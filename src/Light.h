#pragma once

#include "misc/defs.h"
//#include "SceneManager.h"

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

/ TODO: light falloff
/ // TODO: slå ihop ljus?
/
/ struct PointLight {
/    Vec3  position,
/          color;
/    Float32  intensity,
/             radius,
/             specular;
/ };
/ */

// TODO: use SceneManager pointer instead to allow assignment operator?
//
// class Light {
// public:
//    Light() {
//       assert( false && "Wrong constructor!" );
//    }
//
//    Light( Light && ) = delete;
//
//    Light( SceneManager &scene, LightData light ):
//       _id    ( _generate_id() ),
//       _scene ( scene          )
//    {
//       scene.add_light( _id, light );
//    }
//
//    Light( Light const &other ):
//       _id    ( other._id    ),
//       _scene ( other._scene )
//    {
//       _scene.add_light( _id, other.get_data() );
//    }
//
//    ~Light() {
//       _scene.remove_light( _id );
//    }
//
//    Light&     operator=( Light const  & ) = delete; // TODO: ?
//    Light&     operator=( Light       && ) = delete;
//
//    [[nodiscard]] LightData  get_data() const;
//    void                     set_data( LightData const & );
//
//    [[nodiscard]] Uint64     get_id() const;
//
// private:
//    [[nodiscard]] Uint64    _generate_id() const;
//
//    //Mat4        _proj_view_matrix;
//    Uint64        _id;
//    SceneManager &_scene;
// };





class Light {
public:
   enum Type : Uint32 { point       = 0,
                        spot        = 1,
                        directional = 2 };

   struct Data { // nested POD struct
      Light::Type type;

      Vec3        direction,
                  position,
                  color;

      Float32     intensity,
                  radius,
                  degree,
                  specularity;
   };

   Light()                            = delete;
   Light( Light && )                  = delete;
   Light( Light const & )             = delete;
   Light& operator=( Light const  & ) = delete;
   Light& operator=( Light       && ) = delete;

   // NOTE: to be called via SceneManager::instantiate_light()
   Light( std::function<void(Uint64)> on_destruction_callback, Uint64 id, Light::Data &&data ):
      _on_destruction_callback ( on_destruction_callback ),
      _data                    ( std::move(data)         ),
      data                     ( _data                   ),
      id                       ( id                      )
   {}

   // NOTE: to be called via SceneManager::instantiate_light()
   Light( std::function<void(Uint64)> on_destruction_callback, Uint64 id, Light::Data const &data ):
      _on_destruction_callback ( on_destruction_callback ),
      _data                    ( data                    ),
      data                     ( _data                   ),
      id                       ( id                      )
   {}

   ~Light();

   void set_position(    Vec3       && ); // move semantics optimization
   void set_direction(   Vec3       && ); // move semantics optimization
   void set_color(       Vec3       && ); // move semantics optimization
   void set_position(    Vec3 const  & );
   void set_direction(   Vec3 const  & );
   void set_color(       Vec3 const  & );
   void set_intensity(   Float32       );
   void set_radius(      Float32       );
   void set_degree(      Float32       );
   void set_specularity( Float32       );
   void set_type(        Light::Type   );

   [[nodiscard]] Vec3         get_position()    const;
   [[nodiscard]] Vec3         get_direction()   const;
   [[nodiscard]] Vec3         get_color()       const;
   [[nodiscard]] Float32      get_intensity()   const;
   [[nodiscard]] Float32      get_radius()      const;
   [[nodiscard]] Float32      get_degree()      const;
   [[nodiscard]] Float32      get_specularity() const;
   [[nodiscard]] Light::Type  get_type()        const;

   [[nodiscard]] Data const & get_data()        const;    // TODO: decide on this or below
   [[nodiscard]] Uint64       get_id()          const;    // TODO: decide on this or below

private:
   std::function<void(Uint64)> _on_destruction_callback;
   Data                        _data;

public:
   Data   const &data; // <- publicly exposed const data  // TODO: decide on this or above
   Uint64 const id;                                       // TODO: decide on this or above
};
