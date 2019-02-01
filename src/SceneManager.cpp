#include "SceneManager.h"

SharedPtr<ModelInstance> SceneManager::instantiate_model(
   String const                  &model_name,
   SharedPtr<ModelShaderProgram>  shader_program,
   Vec3                           global_position )
{
   // SharedPtr<Model>, SharedPtr<ShaderProgram>
   // r r r t   |  1 0 0 x
   // r r r t   |  0 1 0 y
   // r r r t   |  0 0 1 z
   // 0 0 0 h   |  0 0 0 1
   Mat4 transform       = Mat4(1.0f);
        transform[0][3] = global_position.x;
        transform[1][3] = global_position.y;
        transform[2][3] = global_position.z;

   auto model      = _assman->load_model(model_name); // TODO: std::move(transform)? (LÅG PRIO)

   auto instance   = std::make_shared<ModelInstance>( model, shader_program, transform );
}

void SceneManager::draw() {
   // TODO: sortera instanserna efter ShaderProgram m.h.a. std::partition()
   for ( auto &instance : _instances ) {
      instance.draw();
   }
}
