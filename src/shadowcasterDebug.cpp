#include "shadowcasterDebug.h"

void ShadowcasterDebug::light_caster_debugg_tool_render() {

   if (_dbg_initialied) {
      
      auto t = _dbgdat.SunApe_pos->model_transform;
      t.set_position(*pos);
      _dbgdat.SunApe_pos->set_transform(t);

      auto f = _dbgdat.SunApe_Look->model_transform;
      f.set_position(*this->target);
      _dbgdat.SunApe_Look->set_transform(f);

      //_dbgdat.SunApe_pos->set_transform(_dbgdat.SunApe_pos_modelTrans);
      //_dbgdat.SunApe_Look->set_transform(_dbgdat.sunApe_Look_modeltrans);

      Vec3 up(0.0f, 1.0f, 0.0f);
      int prev = 0;
      for (int i = 0; i < 4; i++) {

         Vec3 lookAt = *this->target - *pos;
         Vec3 tempVertical;
         Vec3 tempHorizontal;
         Vec3 tempPos;
         tempVertical = glm::normalize(glm::cross(lookAt, up));
         tempHorizontal = glm::normalize(glm::cross(-tempVertical, lookAt));
         tempPos = tempVertical * _dbgdat.corners[i] + tempHorizontal * _dbgdat.corners[(i + prev) % (2 + (i % 2))];
         prev += i;

         _dbgdat.corner_trans[i].set_position(*this->target + tempPos);
         _dbgdat.corner[i]->set_transform(_dbgdat.corner_trans[i]);
      }
   }

}

ShadowcasterDebug::ShadowcasterDebug(SharedPtr<Shadowcaster> shadowcaster, AssetManager *asset_manager, SceneManager *scene_manager, Vector<SharedPtr<ModelInstance>> *model_instances, SharedPtr<ShaderProgram> geometry_program, Vec3 *poss, Vec3 *dirr)
   : pos(poss), target(dirr),shadowcaster(shadowcaster)
{

   light_caster_debugg_tool_init(shadowcaster, asset_manager, scene_manager, model_instances, geometry_program,poss,dirr);
}

void ShadowcasterDebug::light_caster_debugg_tool_init(SharedPtr<Shadowcaster> shadowcaster,  AssetManager *asset_manager, SceneManager *scene_manager, Vector<SharedPtr<ModelInstance>> *model_instances, SharedPtr<ShaderProgram> geometry_program, Vec3 *poss, Vec3 *dirr) {


   if (_dbg_initialied == false) {

      this->shadowcaster = shadowcaster;

      auto SunApe_pos_model = asset_manager->load_model("sunApe_pos.obj");
      auto SunApe_look_model = asset_manager->load_model("sunApe_look.obj");
      auto SunApe_corner_model = asset_manager->load_model("sunApe_corner.obj");

      _dbgdat.corner[4];
      _dbgdat.corner_trans[4];
      //int corners[]{ -50, 50, -50, 50 };
       //_left, _right, _bottom, _top 
      
      _dbgdat.corners = this->shadowcaster->getCorners();

      Vec3 up(0.0f, 1.0f, 0.0f);
      int prev = 0;
      for (int i = 0; i < 4; i++) {
         Vec3 target = *dirr - *poss;
         Vec3 tempVertical;
         Vec3 tempHorizontal;
         Vec3 tempPos;
         tempVertical = glm::normalize(glm::cross(target, up));
         tempHorizontal = glm::normalize(glm::cross(-tempVertical, target));
         tempPos = target + tempVertical * _dbgdat.corners[i] + tempHorizontal * _dbgdat.corners[(i + prev) % (2 + (i % 2))];
         prev += i;

         //temp.x = dirr.x + (float)corners[i];
         _dbgdat.corner_trans[i] = Transform(tempPos,
            Vec3(0, 0, 0),
            Vec3(1.0f, 1.0f, 1.0f));

         _dbgdat.corner[i] = scene_manager->instantiate_model(SunApe_corner_model,
            geometry_program,
            _dbgdat.corner_trans[i]);

         model_instances->push_back(_dbgdat.corner[i]);
      }

      Transform SunApe_pos_modelTrans = Transform(*poss,
         Vec3(0, 0, 0),
         Vec3(1.3f, 1.3f, 1.3f));

      SharedPtr<ModelInstance> SunApe_pos = scene_manager->instantiate_model(SunApe_pos_model,
         geometry_program,
         SunApe_pos_modelTrans);

      model_instances->push_back(SunApe_pos);

      Transform sunApe_Look_modeltrans = Transform(*dirr,
         Vec3(0, 0, 0),
         Vec3(1.3f, 1.3f, 1.3f));

      SharedPtr<ModelInstance> SunApe_Look = scene_manager->instantiate_model(SunApe_look_model,
         geometry_program,
         sunApe_Look_modeltrans);

      model_instances->push_back(SunApe_Look);
      
      _dbgdat.SunApe_pos = SunApe_pos;
      _dbgdat.SunApe_Look = SunApe_Look;

      _dbg_initialied = true;
   }
   else {
      //Thank you come again
      std::cout << "Shadowcaster Debuggtool is already initialized!" << std::endl;
   }

}