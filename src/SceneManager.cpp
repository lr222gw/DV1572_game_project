#include "SceneManager.h"

SharedPtr<ModelInstance> SceneManager::instantiate_model(
   SharedPtr<Model>          model,
   SharedPtr<ShaderProgram>  shader_program,
   Transform const&          transform)
{
   // SharedPtr<Model>, SharedPtr<ShaderProgram>
   // r r r t   |  1 0 0 x
   // r r r t   |  0 1 0 y
   // r r r t   |  0 0 1 z
   // 0 0 0 h   |  0 0 0 1
   //Mat4 transform       = Mat4(1.0f);
   //     transform[0][3] = global_position.x;
   //     transform[1][3] = global_position.y;
   //     transform[2][3] = global_position.z;

        

        //transform = glm::rotate(transform,angle, axisTouse)



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


void SceneManager::draw_debug_scene_inspection() {
   ImGui::Begin( "Instances:" ); // begin our Inspection window:
   {  // draw our window GUI components and do I/O:
      Uint32 i = 0;
      for ( auto &e : _instances ) {
         if ( !e.expired() ) {
            auto instance = e.lock();

            auto transform   = instance->model_transform;

            Vec3 rotation  = transform.get_rotation();
            Vec3 position  = transform.get_position();
            Vec3 scale     = transform.get_scale();

            Float32 position_array[3]   { position.x,
                                          position.y,
                                          position.z }; // fulhack

            Float32 scale_array[3] { scale.x,
                                     scale.y,
                                     scale.z }; // fulhack

            // fulhack
            String id = instance->get_model()->get_name() +"::"+ std::to_string(i);

            ImGui::PushID( id.c_str() );
            ImGui::NewLine();
            ImGui::Text( "%s:", id.c_str() );
            ImGui::InputFloat3( "Position", position_array, "%.1f");
            ImGui::SliderAngle( "X rotation", &rotation.x );
            ImGui::SliderAngle( "Y rotation", &rotation.y );
            ImGui::SliderAngle( "Z rotation", &rotation.z );
            ImGui::SliderFloat3( "Scale", scale_array, 0.001f, 100.0f, "%.2f" );
            ImGui::NewLine();

            ImGui::Separator();
            ImGui::PopID();

            Transform new_transform( Vec3( position_array[0],
                                           position_array[1],
                                           position_array[2] ),
                                     rotation,
                                     Vec3( scale_array[0],
                                           scale_array[1],
                                           scale_array[2] ) );

            instance->set_transform( new_transform );
         }
         ++i; // increment counter
      }
   } ImGui::End(); // end our Inspection window
}
