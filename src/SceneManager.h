#pragma once

#include "misc/defs.h"
#include "ModelInstance.h"
#include "ShaderProgram.h"

class SceneManager {
public:
   SharedPtr<ModelInstance> instantiate_model(
      SharedPtr<Model>               model,
      SharedPtr<ShaderProgram>       shader_program,
      Vec3                           global_position={0,0,0} );


   void draw();

private:
   Vector<WeakPtr<ModelInstance>> _instances;
};
