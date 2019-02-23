#pragma once

#include "misc/defs.h"
#include "Config.h"

#include "misc/ImGui/imgui.h"
#include "misc/ImGui/imgui_impl_glfw.h"
#include "misc/ImGui/imgui_impl_opengl3.h"

#include "ModelInstance.h"
#include "ShaderProgram.h"
#include "Transform.h"
#include "Viewport.h"

constexpr Uint32  light_capacity = 32;

// TODO: extract to own file?
enum   LightType : Uint32 { point       = 0,
                            spot        = 1,
                            directional = 2 };

struct LightData {
   LightType   type;

   Vec3        direction,
               position,
               color;

   Float32     intensity,
               radius,
               degree,
               specularity;
};


class SceneManager {
// friend class Handle<ModelInstance>;
// friend class Handle<ParticleSystem>;
// friend class Handle<Light>;
public:
   SharedPtr<ModelInstance> instantiate_model(
      SharedPtr<Model>          model,
      SharedPtr<ShaderProgram>  shader_program,
      Transform const &         transform);

   void                    add_light( Uint64 id, LightData );
   [[nodiscard]] LightData get_light_data( Uint64 id ) const; // NOTE! should only be used by Light's constructor (TODO: private+friend?)
   void                    set_light_data( Uint64 id, LightData );
   void                    remove_light( Uint64 id );         // NOTE! should only be used by Light's destructor (TODO: private+friend?)
   void                    draw( Viewport & );
   void                    draw_debug_scene_inspection();


   // Handle<Light>          add_light          ( Light          && );
   // Handle<ModelInstance>  add_model          ( ModelInstance  && );
   // Handle<ParticleSystem> add_particle_system( ParticleSystem && );

   SceneManager( SharedPtr<ShaderProgram> geometry_pass, SharedPtr<ShaderProgram> lighting_pass );

private:

   Uint32 _find_light_index( Uint64 id ) const;
   SharedPtr<ShaderProgram>        _lighting_shader_program;
   SharedPtr<ShaderProgram>        _geometry_shader_program;
   Vector<WeakPtr<ModelInstance>>  _instances;
   Array<LightData,light_capacity> _light_data;
   Array<Uint64,light_capacity>    _ids;        // used to ensure the correct removal of lights
   Uint32                          _num_lights; // how much of the light capacity is used

   void _lights_to_gpu();
   void _render_to_quad();
};



// template <class T>
// class Handle {
// public:
//    typedef T Type;
//
//    Handle( SceneManager &context, Uint64 id ):
//       _scene_man ( context ),
//       id         ( id      ),
//    {}
//
//    ~Handle() {
//       _scene_man->_remove(this);
//    }
//
//    T& operator*() {
//       return _scene_man->_get(this);
//    }
//
//
//    Uint64 const id;
//
// private:
//    SceneManager &_scene_man;
// };
