#include "SceneManager.h"

SharedPtr<ModelInstance> SceneManager::instantiate_model(
   SharedPtr<Model>          model,
   SharedPtr<ShaderProgram>  shader_program,
   Vec3                      global_position,
   Vec3                      scale,
   Vec3                      rotation_axis)
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

        transform = glm::rotate(transform,angle, axisTouse)

        //transform[0][2] = scale.x;
        //transform[1][2] = scale.y;
        //transform[2][2] = scale.z;
        //transform[0][1] = rotation.x;
        //transform[1][1] = rotation.y;
        //transform[2][1] = rotation.z;
        //glm::rotation();

   //auto model      = _assman->load_model(model_name); // TODO: std::move(transform)? (LÅG PRIO)
   auto instance_ptr = std::make_shared<ModelInstance>(model, shader_program, transform);
   _instances.push_back(instance_ptr);

   return instance_ptr;
}

void SceneManager::draw() {
   // TODO: sortera instanserna efter ShaderProgram m.h.a. std::partition()
   for ( auto &instance : _instances ) {
      
      if (!instance.expired()) {
         instance.lock()->draw();
      }      
   }
}
