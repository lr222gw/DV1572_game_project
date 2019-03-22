#pragma once

#include "misc/defs.h"
#include "Config.h"

#include "misc/ImGui/imgui.h"
#include "misc/ImGui/imgui_impl_glfw.h"
#include "misc/ImGui/imgui_impl_opengl3.h"

//*SSAO*/#include "SuperSampledAmbientOcclusion.h"

#include "ModelInstance.h"
#include "ShaderProgram.h"
#include "Transform.h"

#include "Shadowcaster.h"
#include "ParticleSystem.h"
#include "Light.h"

#include "Viewport.h"


constexpr Uint32  light_capacity = 32; // TODO: refactor into config

class SceneManager {
   friend class DebugGUI;
// friend class Handle<ModelInstance>;
// friend class Handle<ParticleSystem>;
// friend class Handle<Light>;
public:
   SharedPtr<ModelInstance> instantiate_model( SharedPtr<Model>
                                             , SharedPtr<ShaderProgram>
                                             , Transform const &
                                             , Bool has_tessellation_enabled = false
                                             #ifdef   _DEBUG
                                             , Bool is_a_debug_element       = false
                                             #endif /*DEBUG*/
                                             );


   UniquePtr<Viewport> instantiate_viewport( Vec3 position, GLFWwindow *, SharedPtr<ShaderProgram>, Float32 fov = Config::fov_rad );

   SharedPtr<Light> instantiate_light( Light::Data );

   void instantiate_particle_system( WeakPtr<ParticleSystem> ); /* @TAG{PS} */

// [[nodiscard]] LightData get_light_data( Uint64 id ) const; // NOTE! should only be used by Light's constructor (TODO: private+friend?)
//   void                    set_light_data( Uint64 id, LightData );
   // void                    remove_light( Uint64 id );         // NOTE! should only be used by Light's destructor (TODO: private+friend?)
   void update( Float32 delta_time_ms );
   void draw( Viewport & );
   void draw_debug_scene_inspection();

   // Handle<Light>          add_light          ( Light          && );
   // Handle<ModelInstance>  add_model          ( ModelInstance  && );
   // Handle<ParticleSystem> add_particle_system( ParticleSystem && );

   void set_shadowcasting( SharedPtr<Shadowcaster> light );
   void _init_depth_map_FBO(); // TODO: rename
   void use_depth_map_FBO();   // TODO: rename
   void update_shadowmap();

   SceneManager( SharedPtr<ShaderProgram> geo_pass
               , SharedPtr<ShaderProgram> geo_pass_tessellated
               , SharedPtr<ShaderProgram> light_pass
               , SharedPtr<ShaderProgram> shadow_depth
               , SharedPtr<ShaderProgram> particle_shader    /* @TAG{PS} */
            #ifdef _DEBUG
               , SharedPtr<ShaderProgram> dbg_line_program
            #endif /*DEBUG*/
   //* SSAO */ , SharedPtr<ShaderProgram> ssao_main_shader,
   //* SSAO */ , SharedPtr<ShaderProgram> ssao_blur_shader
               );

   SceneManager()                                       = delete;
   SceneManager(SceneManager const & )                  = delete;
   SceneManager(SceneManager      && )                  = delete;
   SceneManager const& operator=(SceneManager const & ) = delete;
   SceneManager const& operator=(SceneManager      && ) = delete;
  ~SceneManager() {};

   Uint32 get_object_id_at_pixel( Uint32 x, Uint32 y, Viewport & ) const;

   SharedPtr<ModelInstance> get_instance_ptr( Uint32 obj_id );

private:
   // TODO: set in SceneManager() to facilitate (de)serialization
   Uint64 _next_light_id = 1;
   Uint64 _next_model_id = 1;

   Uint32 _find_light_index( Uint64 id ) const;

   SharedPtr<ShaderProgram>                  _geometry_shader_program;
   SharedPtr<ShaderProgram>                  _tessellation_shader_program;
   SharedPtr<ShaderProgram>                  _lighting_shader_program;
   SharedPtr<ShaderProgram>                  _shadow_depth_shader;
   SharedPtr<ShaderProgram>                  _particle_shader; /* @TAG{PS} */
//*SSAO*/ SharedPtr<ShaderProgram>                  _ssao_main_shader;
//*SSAO*/ SharedPtr<ShaderProgram>                  _ssao_blur_shader;
#ifdef _DEBUG
   SharedPtr<ShaderProgram>                  _debug_line_shader;
   Vector<WeakPtr<ModelInstance>>            _debug_instances;
#endif /*DEBUG*/


   //DepthMap stuff for Shadowmapping
   Uint32                                    _depth_map_FBO_id;
   HashMap<SharedPtr<Shadowcaster>, Uint32>  _shadow_maps;

   Vector<WeakPtr<ModelInstance>>            _instances;

   Vector<WeakPtr<ParticleSystem>>           _particle_systems;

   HashMap<Uint64, WeakPtr<Light>>           _lights;
   Array<Light::Data, light_capacity>        _light_data;
   Array<      Uint64,light_capacity>        _id_of_light_at; // used to ensure the correct removal of lights
   Uint32                                    _num_lights;     // how much of the light capacity is used

   //If models orientation has been changed, shadowmap needs to be updated
   Bool _should_recalculate_shadowmap = false;
   Bool _should_sort_front_to_back    = false;

   //Listen for transform changes in our ModelInstances


   [[nodiscard]] Uint32 _generate_light_id();
   [[nodiscard]] Uint32 _generate_model_id();
   void                 _light_destruction_listener( Uint64 id );
   void                 _light_change_listener(      Uint64 id );
   void                 _lights_to_gpu();
   void                 _render_to_quad();
   void                 _sort_by_distance( Viewport const & );

   static void   _initialize_quad_vao();
   static GLuint _quad_vao;
   static GLuint _quad_vbo;
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
