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
#include "Shadowcaster.h"

#include "Light.h"

constexpr Uint32  light_capacity = 32; // TODO: refactor into config

class SceneManager {
// friend class Handle<ModelInstance>;
// friend class Handle<ParticleSystem>;
// friend class Handle<Light>;
public:
   SharedPtr<ModelInstance> instantiate_model( SharedPtr<Model>,
                                               SharedPtr<ShaderProgram>,
                                               Transform const & );

   SharedPtr<Light> instantiate_light( Light::Data );


// [[nodiscard]] LightData get_light_data( Uint64 id ) const; // NOTE! should only be used by Light's constructor (TODO: private+friend?)
//   void                    set_light_data( Uint64 id, LightData );
   // void                    remove_light( Uint64 id );         // NOTE! should only be used by Light's destructor (TODO: private+friend?)
   void draw( Viewport & );
   void draw_debug_scene_inspection();

   // Handle<Light>          add_light          ( Light          && );
   // Handle<ModelInstance>  add_model          ( ModelInstance  && );
   // Handle<ParticleSystem> add_particle_system( ParticleSystem && );

   void set_shadowcasting( SharedPtr<Shadowcaster> light );
   void _init_depth_map_FBO(); // TODO: rename
   void use_depth_map_FBO();   // TODO: rename
   void update_shadowmap();

   SceneManager( SharedPtr<ShaderProgram> geo_pass, SharedPtr<ShaderProgram> light_pass, SharedPtr<ShaderProgram> shadow_depth );

   Uint32 get_object_id_at_pixel( Uint32 x, Uint32 y, Viewport & );

   SharedPtr<ModelInstance> get_instance_ptr(Uint32 obj_id);

private:
   Uint64 _next_light_id = 1;

   Uint32 _find_light_index( Uint64 id ) const;
   SharedPtr<ShaderProgram>        _lighting_shader_program;
   SharedPtr<ShaderProgram>        _geometry_shader_program;
   SharedPtr<ShaderProgram>        _shadow_depth_shader;

   //DepthMap stuff for Shadowmapping
   Uint32                                     _depth_map_FBO_id;
   HashMap< SharedPtr<Shadowcaster>, Uint32>  _shadow_maps;// = { _shadowcasters ,  _depth_map_ids };


   Vector<WeakPtr<ModelInstance>>     _instances;

   HashMap<Uint64, WeakPtr<Light>>    _lights;
   Array<Light::Data, light_capacity> _light_data;
   Array<      Uint64,light_capacity> _id_of_light_at; // used to ensure the correct removal of lights
   Uint32                             _num_lights;     // how much of the light capacity is used

   //If models orientation has been changed, shadowmap needs to be updated
   bool _should_recalculate_shadowmap = false;

   //Listen for transform changes in our ModelInstances


   Uint64 _generate_light_id();
   void _light_destruction_listener( Uint64 id );

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
