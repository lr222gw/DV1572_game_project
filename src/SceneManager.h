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

constexpr Uint32 light_capacity = 32;

//TODO: extract to own file
enum   LightType : Uint32 { point = 0, spot = 1, directional = 2 };

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
public:
   SharedPtr<ModelInstance> instantiate_model(
      SharedPtr<Model>          model,
      SharedPtr<ShaderProgram>  shader_program,
      Transform const &         transform);

   void       add_light( Uint64 id, LightData data );
   LightData  get_light_data( Uint64 id ) const; // NOTE! should only be used by Light's constructor (TODO: private+friend?)
   void       set_light_data( Uint64 id, LightData data );
   void       remove_light( Uint64 id );         // NOTE! should only be used by Light's destructor (TODO: private+friend?)
   void       draw( Viewport &view );
   void       draw_debug_scene_inspection();

private:

   Uint32 _find_light_index( Uint64 id ) const;

   Vector<WeakPtr<ModelInstance>>  _instances;
   Array<LightData,light_capacity> _light_data;
   Array<Uint64,light_capacity>    _ids;        // used to ensure the correct removal of lights
   Uint32                          _num_lights; // how much of the light capacity is used
};
