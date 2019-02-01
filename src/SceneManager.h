#pragma once

#include "misc/defs.h"
#include "ModelInstance.h"
#include "ShaderProgram.h"

class SceneManager {

   SharedPtr<ModelInstance> instantiate_model(
      String const                  &model_name,
      SharedPtr<ShaderProgram>       shader_program,
      Vec3                           global_position={0,0,0} );


   void draw();

private:
   Vector<WeakPtr<ModelInstance>> _instances;
};
