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

   glUseProgram(this->_geometry_pass_shader->get_location());

   glBindFramebuffer( GL_FRAMEBUFFER, g_buffer.buffer_loc );
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // toggle wireframe mode if config is set to true
   if ( config.is_wireframe_mode )
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

   view.update();
   auto view_pos = view.get_view().get_position();

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

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // ( 0.4f, 0.6, 1.0, 1.0f );
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glUseProgram(this->_light_pass_shader->get_location());

   ////////////////////
   Uint32 const num_lights{ 8 };
   glUniform1i(this->_light_pass_shader->get_location(), num_lights);

   LightData lights[light_capacity];

   lights[0] = LightData{ LightType::point,
                          Vec3(0.0f,   0.0f,   0.0f),
                          Vec3(10.0f,  10.0f,  10.0f),
                          Vec3(1.0f,   0.0f,   0.0f),
                           1.0,
                          14.0,
                           0.0,
                           1.0 };

   lights[1] = LightData{ LightType::point,
                          Vec3(0.0f,  0.0f,  0.0f),
                          Vec3(1.0f,  4.0f,  5.0f),
                          Vec3(1.0f,  1.0f,  0.0f),
                          1.0,
                          7.0,
                          0.0,
                          1.0 };

   lights[2] = LightData{ LightType::point,
                          Vec3(0.0f,  0.0f,  0.0f),
                          Vec3(2.0f,  1.0f,  5.0f),
                          Vec3(1.0f,  0.0f,  1.0f),
                           1.0,
                          17.0,
                           0.0,
                           1.0 };

   lights[3] = LightData{ LightType::point,
                          Vec3(0.0f,  0.0f,  0.0f),
                          Vec3(1.0f,  5.0f,  6.0f),
                          Vec3(0.0f,  1.0f,  0.0f),
                           1.0,
                          11.0,
                           0.0,
                           1.0 };

   lights[4] = LightData{ LightType::point,
                          Vec3(0.0f,  0.0f,  1.0f),
                          Vec3(3.0f,  3.0f,  1.0f),
                          Vec3(0.0f,  1.0f,  1.0f),
                          1.0,
                          2.0,
                          0.0,
                          1.0 };

   lights[5] = LightData{ LightType::point,
                          Vec3(0.0f,  0.0f,   0.0f),
                          Vec3(1.0f,  2.0f,  10.0f),
                          Vec3(0.0f,  0.0f,   1.0f),
                          1.0,
                          1.0,
                          0.0,
                          1.0 };

   lights[6] = LightData{ LightType::point,
                          Vec3(0.0f,  0.0f,  0.0f),
                          Vec3(10.0f,  0.0f,  5.0f),
                          Vec3(1.0f,  1.0f,  1.0f),
                          1.0,
                          7.0,
                          0.0,
                          1.0 };

   lights[7] = LightData{ LightType::point,
                          Vec3(0.0f,  0.0f,   0.0f),
                          Vec3(10.0f,  5.0f,  10.0f),
                          Vec3(1.0f,  0.3f,   0.5f),
                           1.0,
                          17.0,
                           0.0,
                           1.0 };

   Uint32 static  quad_vao = 0;
   Uint32 static  quad_vbo;
   //////////////////////


   // glUseProgram(shaProg->get_location());
      // a_Mesh.render();
   auto g_buffer_data{ view.get_g_buffer() };

   // glUseProgram( lightProg->get_location() );


   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, g_buffer_data.pos_tex_loc);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, g_buffer_data.nor_tex_loc);
   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_2D, g_buffer_data.spe_tex_loc);
   glActiveTexture(GL_TEXTURE3);
   glBindTexture(GL_TEXTURE_2D, g_buffer_data.alb_tex_loc); //TODO:P Denna är den enda som gör något...

   for (Uint32 i = 0; i < num_lights; ++i) { //TODO:P Endast en ljuskälla ger samma resultat som att använda alla...
      LightData &ld = lights[i];
      glUniform1ui(glGetUniformLocation(this->_light_pass_shader->get_location(), ("lights[" + std::to_string(i) + "].type").c_str()), ld.type);
      glUniform3fv(glGetUniformLocation(this->_light_pass_shader->get_location(), ("lights[" + std::to_string(i) + "].dir").c_str()), 1, glm::value_ptr(ld.direction));
      glUniform3fv(glGetUniformLocation(this->_light_pass_shader->get_location(), ("lights[" + std::to_string(i) + "].pos").c_str()), 1, glm::value_ptr(ld.position));
      glUniform3fv(glGetUniformLocation(this->_light_pass_shader->get_location(), ("lights[" + std::to_string(i) + "].rgb").c_str()), 1, glm::value_ptr(ld.color));
      glUniform1f(glGetUniformLocation(this->_light_pass_shader->get_location(), ("lights[" + std::to_string(i) + "].intensity").c_str()), ld.intensity);
      glUniform1f(glGetUniformLocation(this->_light_pass_shader->get_location(), ("lights[" + std::to_string(i) + "].radius").c_str()), ld.radius);
      glUniform1f(glGetUniformLocation(this->_light_pass_shader->get_location(), ("lights[" + std::to_string(i) + "].degree").c_str()), ld.degree);
      glUniform1f(glGetUniformLocation(this->_light_pass_shader->get_location(), ("lights[" + std::to_string(i) + "].specularity").c_str()), ld.specularity);
   }
   glUniform1ui(glGetUniformLocation(this->_light_pass_shader->get_location(), "num_lights"), num_lights);
   glUniform3fv(glGetUniformLocation(this->_light_pass_shader->get_location(), "view_pos"), 1, glm::value_ptr(view_pos));
   glUniform1ui(glGetUniformLocation(this->_light_pass_shader->get_location(), "render_mode"), (Uint32)config.render_mode);

   if (0 == quad_vao) {
      Float32 quad_verts[] = {
         //   X      Y     Z       U     V
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
      };

      // setup plane VAO
      glGenVertexArrays(1, &quad_vao);
      glGenBuffers(1, &quad_vbo);
      glBindVertexArray(quad_vao);

      glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);

      glBufferData(GL_ARRAY_BUFFER,
         sizeof(quad_verts),
         &quad_verts,
         GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);

      glVertexAttribPointer(0,
         3,
         GL_FLOAT,
         GL_FALSE,
         5 * sizeof(Float32),
         (void*)0);

      glEnableVertexAttribArray(1);

      glVertexAttribPointer(1,
         2,
         GL_FLOAT,
         GL_FALSE,
         5 * sizeof(Float32),
         (void*)(3 * sizeof(Float32)));
   }
   glBindVertexArray(quad_vao);

   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

   glBindVertexArray(0);


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

SceneManager::SceneManager(SharedPtr<ShaderProgram> geo_pass, SharedPtr<ShaderProgram> light_pass)
{   
   this->_geometry_pass_shader = geo_pass;
   this->_light_pass_shader = light_pass;
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
