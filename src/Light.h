#pragma once

#include "misc/defs.h"

#ifdef _DEBUG
   #include "LightDebugger.h"
#endif

//#include "SceneManager.h"

/* // TODO: refactor back from SceneManager.h?
struct DirLight {
   Vec3     direction,
            color;
   Float32  intensity,
            specular;
};

struct SpotLight {
   Vec3     position,
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
/    Vec3     position,
/             color;
/    Float32  intensity,
/             radius,
/             specular;
/ };
/ */

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

   // TODO: move definition into source
   // NOTE: to be called via SceneManager::instantiate_light()
   Light( std::function<void(Uint64)>   on_destruction_callback,
          std::function<void(Uint64)>   on_change_callback,
          Uint64                        id,
          Light::Data                 &&data )
   :
      _on_destruction_callback ( on_destruction_callback ),
      _on_change_callback      ( on_change_callback      ),
      _data                    ( std::move(data)         ),
      data                     ( _data                   ),
      id                       ( id                      )
   {
   #ifdef _DEBUG
      _update_debug_circle();
   #endif /*DEBUG*/
   }

   // TODO: move definition into source
   // NOTE: to be called via SceneManager::instantiate_light()
   Light( std::function<void(Uint64)>  on_destruction_callback,
          std::function<void(Uint64)>  on_change_callback,
          Uint64                       id,
          Light::Data                 &data ) // change to non-const, need pointers in ShadowcasterDebug
   :
      _on_destruction_callback ( on_destruction_callback ),
      _on_change_callback      ( on_change_callback      ),
      _data                    ( data                    ),
      data                     ( _data                   ),
      id                       ( id                      )
   {
   #ifdef _DEBUG
      _update_debug_circle();
   #endif /*DEBUG*/
   }

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
   std::function<void(Uint64)> _on_change_callback;
   Data                        _data;

public:
   //TODO: data is set to non-const, used in ShadowMapDebugger
   Data         &data; // <- publicly exposed const data   // TODO: decide on this or above
   Uint64 const  id;

   #ifdef _DEBUG
   public:
      Circle  debug_circle;

   private:
      void _update_debug_circle() {
         debug_circle.set_hidden(   _data.type == Light::Type::point );
         debug_circle.set_radius(   _data.radius   );
         debug_circle.set_position( _data.position );
         debug_circle.set_color( { GLubyte( _data.color[0] * 255U ),
                                   GLubyte( _data.color[0] * 255U ),
                                   GLubyte( _data.color[0] * 255U )  } );
      }
   #endif /*DEBUG*/                              // TODO: decide on this or above
};
