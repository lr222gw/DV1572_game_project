#include "SceneManager.h"

SharedPtr<ModelInstance> SceneManager::instantiate_model(
   SharedPtr<Model>          model,
   SharedPtr<ShaderProgram>  shader_program,
   Transform const&          transform)
{
   // construct return value (shared pointer):         TODO: switch to UniquePtr..?
   auto instance_ptr { std::make_shared<ModelInstance>(model, shader_program, transform) };

   // add a weak pointer to the scene manager's instance list before returning:
   _instances.push_back( instance_ptr );

   return instance_ptr;
}


void SceneManager::draw( Viewport &view ) {
   auto &g_buffer = view.get_g_buffer(); // test
   glBindFramebuffer( GL_FRAMEBUFFER, g_buffer.buffer_loc );
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // toggle wireframe mode if config is set to true
   if ( config.is_wireframe_mode )
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

   view.update();

// 1. Geometry Pass:
   // TODO: sortera instanserna efter ShaderProgram m.h.a. std::partition()
   for ( auto &instance : _instances )
      if (!instance.expired())
         instance.lock()->draw();

   // disabling wireframe rendering so that the quad will render after the lighting pass
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

// 2. Lighting pass:
   glBindFramebuffer( GL_FRAMEBUFFER, 0 );
   // TODO: refactor lighting pass code here
}



void SceneManager::draw_debug_scene_inspection() {
   ImGui::Begin( "Instances:" ); // begin our Inspection window:
   {  // draw our window GUI components and do I/O:
      Uint32 i = 0;
      for ( auto &e : _instances ) {
         if ( !e.expired() ) {
            auto instance = e.lock();

            auto transform   = instance->model_transform;

            // Vec3 rotation  = transform.get_rotation();
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
            // ImGui::SliderAngle( "X rotation", &rotation.x );
            // ImGui::SliderAngle( "Y rotation", &rotation.y );
            // ImGui::SliderAngle( "Z rotation", &rotation.z );
            ImGui::InputFloat3( "Scale", scale_array, "%.2f" );
            ImGui::NewLine();

            ImGui::Separator();
            ImGui::PopID();

            Transform new_transform( Vec3( position_array[0],
                                           position_array[1],
                                           position_array[2] ),
                                /* temp */ transform.get_rotation(),
                                     Vec3( scale_array[0],
                                           scale_array[1],
                                           scale_array[2] ) );

            instance->set_transform( new_transform );
         }
         ++i; // increment counter
      }
   } ImGui::End(); // end our Inspection window
}

SceneManager::SceneManager(SharedPtr<ShaderProgram> lightPass)
{   
   this->_light_pass_shader = lightPass;
}



// NOTE! should only be used by Light's constructor (TODO: private+friend?)
void SceneManager::add_light( Uint64 id, LightData data ) {
   _light_data[_num_lights] = data;
   _ids[_num_lights]        = id;
   ++_num_lights; // increment counter
}

LightData SceneManager::get_light_data( Uint64 id ) const {
   auto index = _find_light_index(id);
   return _light_data[index];
}

void SceneManager::set_light_data( Uint64 id, LightData data ) {
   auto index = _find_light_index(id);
   _light_data[index] = data;
}

// NOTE! should only be used by Light's destructor (TODO: private+friend?)
void SceneManager::remove_light( Uint64 id ) {
   auto index = _find_light_index(id);

   --_num_lights; // decrement counter

   // swap target element with last element
   // (the removed item will be moved out of the active partition of the array)
   std::swap( _light_data[index], _light_data[_num_lights] );
   std::swap(        _ids[index],        _ids[_num_lights] );
}


Uint32 SceneManager::_find_light_index( Uint64 id ) const {
   auto index = -1;
   // find index of target id
   for ( auto i = 0;  index < _num_lights;  ++index )
      if ( _ids[i] == id)
         index = i;
   assert( index != -1 && "Bug! There should only be one entry point and exit point for every light instance" );
   return index;
}
