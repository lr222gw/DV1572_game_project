#pragma once

#include "misc/defs.h"
#include "ModelInstance.h"
#include "ShaderProgram.h"
#include "Transform.h"

class SceneManager {
public:
   SharedPtr<ModelInstance> instantiate_model(
      SharedPtr<Model>               model,
      SharedPtr<ShaderProgram>       shader_program,
      Transform const &                 transform);


   void draw();

   void draw_debug_scene_inspection();

private:
   Vector<WeakPtr<ModelInstance>> _instances;
};
