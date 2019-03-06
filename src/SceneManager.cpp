#include "SceneManager.h"

SharedPtr<ModelInstance> SceneManager::instantiate_model(
   SharedPtr<Model>          model,
   SharedPtr<ShaderProgram>  shader_program,
   Transform const&          transform)
{
   auto callback_lambda = [=]() { _should_recalculate_shadowmap = true; };
   // construct return value (shared pointer):
   auto instance_ptr = // TODO: switch to UniquePtr..?
      std::make_shared<ModelInstance>( model,
                                       shader_program,
                                       transform,
                                       callback_lambda );

   // add a weak pointer to the scene manager's instance list before returning:
   _instances.push_back( instance_ptr );

   return instance_ptr;
}


Uint64 SceneManager::_generate_light_id() {
   return _next_light_id++;
}

void SceneManager::_light_change_listener( Uint64 id ) {
   auto light = _lights[id];
   if ( !light.expired()  &&  light.lock()->get_type() == Light::Type::directional )
      update_shadowmap();

   int match_index = -1;
   for ( auto index=0;  index<_num_lights;  ++index )
      if ( _id_of_light_at[index] == id )
         match_index = index;
   if ( match_index != -1 ) {
      auto light_match         = light.lock();
      auto new_data            = light_match->get_data();
      _light_data[match_index] = new_data;
   }
}

void SceneManager::_light_destruction_listener( Uint64 id ) {
   int match_index = -1;
   for ( auto index=0;  index<_num_lights;  ++index )
      if ( _id_of_light_at[index] == id )
         match_index = index;

   // if the light was in the light data buffer, remove it by swapping
   if ( -1 != match_index ) { // the entry with the last entry and decrement count
      Light::Data  temp_buffer = _light_data[match_index];
      _light_data[match_index] = _light_data[--_num_lights];
      _light_data[_num_lights] = temp_buffer;
      // TODO: (if buffer for over-capacity lights gets added): check for new lights to insert
   }

   // TODO: "else { /* remove from over-capacity buffer */ }""
}

SharedPtr<Light> SceneManager::instantiate_light( Light::Data data )
{
   auto result = std::make_shared<Light>( std::bind( &SceneManager::_light_destruction_listener,
                                                     this,
                                                     std::placeholders::_1 ),
                                          std::bind( &SceneManager::_light_change_listener,
                                                     this,
                                                     std::placeholders::_1 ),
                                          _generate_light_id(),
                                          std::move(data) );
   _lights[result->id] = result; // adding tdo instance table

   if ( _num_lights < light_capacity ) { // adding
      _light_data     [_num_lights] = result->data;
      _id_of_light_at [_num_lights] = result->id;
      ++_num_lights;
   }
   else assert( false && "Add buffer for inactive lights. " );
   return result;
}

// TODO: use ShaderProgram::use()
void SceneManager::draw( Viewport &view ) {
   auto &g_buffer = view.get_g_buffer();

   auto lighting_pass_loc = _lighting_shader_program->get_location();
   auto geometry_pass_loc = _geometry_shader_program->get_location();

   //TODO: Make modelinstance supply unique ID to Callback Function and then in CAllback function compare the boundingbox of the modelinstance with the frustrum of all the active shadowcasters and recalculate shadowmap for any intersections
   if ( _should_recalculate_shadowmap ) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      this->update_shadowmap();
      _should_recalculate_shadowmap = false;
   }

   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   _geometry_shader_program->use(); // glUseProgram( geometry_pass_loc );

   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, g_buffer.buffer_loc );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // toggle wireframe mode if config is set to true
   if ( config.is_wireframe_mode )
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

   //Send view_pos to g_buffer.geom shader
   view.update();
   auto view_pos = view.get_view().get_position();
   glUniform3fv( glGetUniformLocation( geometry_pass_loc, "view_pos"),
                 1,
                 glm::value_ptr(view_pos));


   // 1. Geometry Pass:
   // TODO: sortera instanserna efter ShaderProgram m.h.a. std::partition()
   for ( auto &instance : _instances )
      if ( !instance.expired() )
         instance.lock()->draw();

   // disabling wireframe rendering so that the quad will render after the lighting pass
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

// 2. Lighting pass:
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0 );
   // TODO: refactor lighting pass code here

   glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   _lighting_shader_program->use(); // glUseProgram( lighting_pass_loc );

   ////////////////////

   auto g_buffer_data { view.get_g_buffer() };

   glActiveTexture( GL_TEXTURE0 );
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.pos_tex_loc );
   glActiveTexture( GL_TEXTURE1 );
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.nor_tex_loc );
   glActiveTexture( GL_TEXTURE2 );
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.spe_tex_loc );
   glActiveTexture( GL_TEXTURE3) ;
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.alb_tex_loc );
   glActiveTexture( GL_TEXTURE4) ;
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.emi_tex_loc );
   glActiveTexture( GL_TEXTURE5);
   glBindTexture(	  GL_TEXTURE_2D, g_buffer_data.pic_tex_loc );

   glUniform3fv( glGetUniformLocation( lighting_pass_loc, "view_pos"),
                 1,
                 glm::value_ptr(view_pos));

   for ( auto &e : _shadow_maps ) {
      glUniformMatrix4fv(
         glGetUniformLocation(lighting_pass_loc,
            "lightmatrix"),
         1,
         GL_FALSE,
         glm::value_ptr(e.first->get_matrix()));
      // Mat4 ello = e.first->get_matrix();
      glActiveTexture(GL_TEXTURE6);
      glBindTexture(GL_TEXTURE_2D, e.second);

      //glUniform1i(glGetUniformLocation(_light_pass_shader->get_location(), "shadowMap"), 4);
   }


   _lights_to_gpu();
   _render_to_quad();
}

void SceneManager::_render_to_quad() {

   static Uint32  quad_vao = 0;
   static Uint32  quad_vbo;

   if ( 0 == quad_vao ) {
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

      //Screen 2D Pos
      glVertexAttribPointer(0,
         3,
         GL_FLOAT,
         GL_FALSE,
         5 * sizeof(Float32), // X,Y,Z,U,V = 5 Float32 channels
         (void*)0);

      glEnableVertexAttribArray(1);

      //Screen 2D uv
      glVertexAttribPointer( 1,
                             2,
                             GL_FLOAT,
                             GL_FALSE,
                             5 * sizeof(Float32),
                             (void*)(3 * sizeof(Float32)));
   }
   glBindVertexArray( quad_vao );

   glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

   glBindVertexArray(0);
}


void SceneManager::draw_debug_scene_inspection() {
   if ( !config.is_imgui_toggled )
      return;

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

            Float32 position_array[3] { position.x,
                                        position.y,
                                        position.z }; // temp

            Float32 scale_array[3] { scale.x,
                                     scale.y,
                                     scale.z }; // temp

            Vec3 rotation(0.0f);

            // temp
            String id = instance->get_model()->get_name() +"::"+ std::to_string(i);

            ImGui::PushID( id.c_str() );
            ImGui::NewLine();
            ImGui::Text( "%s:", id.c_str() );
            ImGui::InputFloat3( "Position", position_array, "%.1f");

            ImGui::SliderAngle( "X rotation", &rotation.x );
            ImGui::SliderAngle( "Y rotation", &rotation.y );
            ImGui::SliderAngle( "Z rotation", &rotation.z );

            ImGui::InputFloat3( "Scale", scale_array, "%.2f" );
            ImGui::NewLine();

            ImGui::Separator();
            ImGui::PopID();

            // Vec3 oldPos = position;

            Transform trans = Transform(Vec3(0.0f, 0.0f, 0.0f));
            //instance->set_transform(trans);
            //transform = instance->model_transform;

            //instance->set_transform(trans);


            //instance->set_transform(trans);
            transform = instance->model_transform;

            transform.set_position(Vec3(0.0f,0.0f,0.0f));

            instance->set_transform(transform);
            transform = instance->model_transform;

            //glm::rotation(Mat4(1.0f), rotation);
            transform.set_rotation(Vec3(1.0f, 0.0f, 0.0f), rotation.x);
            transform.set_rotation(Vec3(0.0f, 1.0f, 0.0f), rotation.y);
            transform.set_rotation(Vec3(0.0f, 0.0f, 1.0f), rotation.z);

            //transform.set_rotation(transform.get_rotation());
            //position = oldPos;
            //transform.get_rotation();
            //instance->set_transform(transform);


            Transform new_transform( Vec3( position_array[0],
                                           position_array[1],
                                           position_array[2] ),
                                /* temp */ transform.get_rotation(),
                                     Vec3( scale_array[0],
                                           scale_array[1],
                                           scale_array[2] ) );

            //Transform new_transform(Vec3(0.0f, 0.0f,0.0f));
           ////new_transform.set_rotation(Transform::make_rotation(,));
            //instance->set_transform(new_transform);
            //new_transform.set_position(Vec3(position_array[0],
            //   position_array[1],
            //   position_array[2]));

            instance->set_transform(new_transform);

         }
         ++i; // increment counter
      }
   } ImGui::End(); // end our Inspection window
}

void SceneManager::set_shadowcasting(SharedPtr<Shadowcaster> light)
{
   //TODO: If-statement to check validity of light, is it directional?
   use_depth_map_FBO();

   Uint32 depthMap, width, height;
   glGenTextures(1, &depthMap);

   width  = 1024;
   height = 1024; //TODO: Enable SetSize of Width and height

   glBindTexture(GL_TEXTURE_2D, depthMap);

   glTexImage2D(GL_TEXTURE_2D,
      0,
      GL_DEPTH_COMPONENT,
      width,
      height,
      0,
      GL_DEPTH_COMPONENT,
      GL_FLOAT,
      NULL);

   glTexParameteri(GL_TEXTURE_2D,
      GL_TEXTURE_MIN_FILTER,
      GL_NEAREST);

   glTexParameteri(GL_TEXTURE_2D,
      GL_TEXTURE_MAG_FILTER,
      GL_NEAREST);

   glTexParameteri(GL_TEXTURE_2D,
      GL_TEXTURE_WRAP_S,
      GL_CLAMP_TO_BORDER);

   glTexParameteri(GL_TEXTURE_2D,
      GL_TEXTURE_WRAP_T,
      GL_CLAMP_TO_BORDER);

//   glTexParameteri(GL_TEXTURE_2D,
//      GL_CLAMP_TO_BORDER, //  LOWE!
//      GL_CLAMP_TO_BORDER);

   float borderColor[] = { 0.0,0.0,0.0, 0.0 };
   glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

   //Uint32 AttatchmentNmbr = _shadow_maps.size();

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);

   _shadow_maps.emplace(light, depthMap);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneManager::_init_depth_map_FBO()
{
   glGenFramebuffers(1, &_depth_map_FBO_id);
}

void SceneManager::use_depth_map_FBO()
{
   glBindFramebuffer(GL_FRAMEBUFFER, _depth_map_FBO_id);
}

void SceneManager::update_shadowmap()
{
   if (_shadow_maps.size() == 0) {
      return;
   }

   //glBindFramebuffer(GL_FRAMEBUFFER, this->_depth_map_FBO_id);
   use_depth_map_FBO();
   glUseProgram(_shadow_depth_shader->get_location());

   GLuint count = 0;
   GLint oldSize[4];
   glGetIntegerv(GL_VIEWPORT, oldSize);


   for (auto &e : _shadow_maps) {
      //_shadowcasters[i]
      //send lightMatrix to
      glUniformMatrix4fv(
         glGetUniformLocation(_shadow_depth_shader->get_location(),
            "lightmatrix"),
         1,
         GL_FALSE,
         glm::value_ptr(e.first->get_matrix()));

      glViewport(0, 0, 1024, 1024);//TODO: do not hardcode, decide based on Shadowcaster


      glClear(GL_DEPTH_BUFFER_BIT);

      //TODO: Increment GL_TEXTURE
      //glActiveTexture(GL_TEXTURE0/*+ count*/);

      //Render from light pov
      for (auto &instance : _instances) {
         if (!instance.expired()) {
            SharedPtr<ShaderProgram> previous = instance.lock()->get_shader_program();
            instance.lock()->set_shader_program(this->_shadow_depth_shader);
            instance.lock()->draw();
            instance.lock()->set_shader_program(previous);
         }
      }

      count++;
   }
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glViewport(0, 0, oldSize[2], oldSize[3]);

}

SceneManager::SceneManager( SharedPtr<ShaderProgram> geo_pass,
                            SharedPtr<ShaderProgram> light_pass,
                            SharedPtr<ShaderProgram> shadow_depth )
:
   _lighting_shader_program ( light_pass   ),
   _geometry_shader_program ( geo_pass     ),
   _shadow_depth_shader     ( shadow_depth ),
   _num_lights              ( 0 )
{
   _init_depth_map_FBO();
}

//SceneManager::SceneManager( SharedPtr<ShaderProgram> geometry_pass,
//                            SharedPtr<ShaderProgram> lighting_pass )
//{
//   _geometry_shader_program = geometry_pass;
//   _lighting_shader_program = lighting_pass;
//}

// TODO: refactor light instances into classes that RAII wrap their lifetimes

// NOTE! should only be used by Light's constructor (TODO: private+friend?)
/*
void SceneManager::add_light( Uint64 id, LightData data ) {
   _light_data[_num_lights] = data;
   _ids[_num_lights]        = id;
   ++_num_lights; // increment counter
}
*/

/*
LightData SceneManager::get_light_data( Uint64 id ) const {
   auto index = _find_light_index(id);
   return _light_data[index];
}
*/

/*
void SceneManager::set_light_data( Uint64 id, LightData data ) {
   auto index = _find_light_index(id);
   _light_data[index] = data;
}*/

// NOTE! should only be used by Light's destructor (TODO: private+friend?)
/*
void SceneManager::remove_light( Uint64 id ) {
   auto index = _find_light_index(id);

   --_num_lights; // decrement counter

   // swap target element with last element
   // (the removed item will be moved out of the active partition of the array)
   std::swap( _light_data[index], _light_data[_num_lights] );
   std::swap(        _ids[index],        _ids[_num_lights] );
}
*/

/*
Uint32 SceneManager::_find_light_index( Uint64 id ) const {
   auto index = -1;
   // find index of target id
   //for ( auto i = 0;  index < _num_lights;  ++index )
   //   if ( _ids[i] == id)
   //      index = i;

   for ( auto i = 0; i < _num_lights; ++i )
      if ( _ids[i] == id )
         index = i;

   //TODO:P Should this assert be here with these conditions?
   assert( index != -1 && "Invalid index; no match." );
   return index;
}
*/

void SceneManager::_lights_to_gpu() {
   auto lighting_pass_loc = _lighting_shader_program->get_location();

   glUniform1ui( glGetUniformLocation( lighting_pass_loc, "num_lights" ),
                _num_lights );

   for ( Uint32 i = 0;  i < _num_lights;  ++i ) {
      auto light = _light_data[i]; // light data of light at index 'i'
      auto str_i = std::to_string(i); // index 'i' as String

      glUniform1ui( glGetUniformLocation( lighting_pass_loc, ("lights["+str_i+"].type").c_str() ),
                    light.type );

      glUniform3fv( glGetUniformLocation( lighting_pass_loc, ("lights["+str_i+"].dir").c_str() ),
                    1,
                    glm::value_ptr(light.direction) );

      glUniform3fv( glGetUniformLocation( lighting_pass_loc, ("lights["+str_i+"].pos").c_str() ),
                    1,
                    glm::value_ptr(light.position) );

      glUniform3fv( glGetUniformLocation( lighting_pass_loc, ("lights["+str_i+"].rgb").c_str() ),
                    1,
                    glm::value_ptr(light.color) );

      glUniform1f( glGetUniformLocation( lighting_pass_loc, ("lights["+str_i+"].intensity").c_str() ),
                   light.intensity );

      glUniform1f( glGetUniformLocation( lighting_pass_loc, ("lights["+str_i+"].radius").c_str() ),
                   light.radius );

      glUniform1f( glGetUniformLocation( lighting_pass_loc, ("lights["+str_i+"].degree").c_str() ),
                   light.degree );

      glUniform1f( glGetUniformLocation( lighting_pass_loc, ("lights["+str_i+"].specularity").c_str() ),
                   light.specularity );
   }

   glUniform1ui( glGetUniformLocation( lighting_pass_loc, "num_lights"),
                 _num_lights );

   glUniform1ui( glGetUniformLocation( lighting_pass_loc, "render_mode"),
                 (Uint32)config.render_mode );
}

Uint32 SceneManager::get_object_id_at_pixel(Uint32 x, Uint32 y, Viewport &view)
{
	_geometry_shader_program->use();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, view.get_g_buffer().buffer_loc);
	glReadBuffer(GL_COLOR_ATTACHMENT5);
	//glUseProgram(_geometry_shader_program)

//	Uint32 pixel_info[4]{};
	struct pixel_info_struct
	{
		Float32 x;
		Float32 y;
		Float32 z;
		Float32 w;
	};
	pixel_info_struct pixel_info;
//
//	glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_INT, (void*)&pixel_info);
//
//	Uint32 obj_id = (pixel_info[0] & 0xFF << 24)
//                 + (pixel_info[1] & 0xFF << 16)
//                 + (pixel_info[2] & 0xFF <<  8)
//                 + (pixel_info[3] & 0xFF <<  0); // TODO: validate that we get the correct ids

	//Uint8 pixel_info[4] {};

	glReadPixels( x, y, 1, 1, GL_RGBA, GL_FLOAT, &pixel_info );

	// probably don't need the & 0xFF since we send up in increments of 8 bytes
	Uint32 obj_id = ( ((Int32)(pixel_info.x * 255) & 0xFF) <<  0)
                  + ( ((Int32)(pixel_info.y * 255) & 0xFF) <<  8)
                  + ( ((Int32)(pixel_info.z * 255) & 0xFF) << 16)
                  + ( ((Int32)(pixel_info.w * 255) & 0xFF) << 24);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return obj_id;
}

SharedPtr<ModelInstance> SceneManager::get_instance_ptr( Uint32 obj_id ) {
	for ( auto &e : _instances ) {
		if ( !e.expired() ) {
			auto e_ptr = e.lock();
			if ( e_ptr->id == obj_id )
				return e_ptr;
		}
		else
		{
			return nullptr;
		}
	}
	//assert( false && "[ERROR] Instance of id no longer exists." );
	std::cout << "no model hit" << std::endl;
}



/*
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
                           1.0 };*/
