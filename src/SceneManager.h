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
      Vec3                           global_position={0,0,0},
      Vec3                           scale = { 0,0,0 },
      Vec3                           rotation = { 0,0,0 });


   void draw();

private:
   Vector<WeakPtr<ModelInstance>> _instances;
};
