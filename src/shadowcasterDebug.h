#pragma once

#include "misc/defs.h"
#include "AssetManager.h"
#include "SceneManager.h"
#include "ModelInstance.h"
#include "Shadowcaster.h"

struct debuggData {
   //SharedPtr<Model> SunApe_pos_model;
   //Transform SunApe_pos_modelTrans;
   SharedPtr<ModelInstance> SunApe_pos;

   //SharedPtr<Model> SunApe_look_model;
   //Transform sunApe_Look_modeltrans;
   SharedPtr<ModelInstance> SunApe_Look;

   //SharedPtr<Model> SunApe_corner_model;
   SharedPtr<ModelInstance> corner[4];
   Transform corner_trans[4];
   Array<Float32,4> corners;
};

class ShadowcasterDebug {
public:

   Vec3 *pos;
   Vec3 *target;

   SharedPtr<Shadowcaster> shadowcaster;
   
   //NOTE: This Constructor is only for initialization of an empty ShadowcasterDebug instance, 
   //it should only be used if an instance is needed to be defined before a DBG_DIABLE_START! 
   ShadowcasterDebug(); 

   ShadowcasterDebug(SharedPtr<Shadowcaster> shadowcaster, AssetManager *asset_manager, SceneManager *scene_manager, Vector<SharedPtr<ModelInstance>> *model_instances, SharedPtr<ShaderProgram> geometry_program, Vec3 *poss, Vec3 *dirr);

   void light_caster_debugg_tool_init(SharedPtr<Shadowcaster> shadowcaster, AssetManager *asset_manager, SceneManager *scene_manager, Vector<SharedPtr<ModelInstance>> *model_instances, SharedPtr<ShaderProgram> geometry_program, Vec3 *poss, Vec3 *dirr);
   void light_caster_debugg_tool_render();

private:
   debuggData        _dbgdat;
   bool              _dbg_initialied = false; //TODO: does this work?
};