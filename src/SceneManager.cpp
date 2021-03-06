#include "SceneManager.h"

GLuint SceneManager::_quad_vao {};
GLuint SceneManager::_quad_vbo {};

SharedPtr<ModelInstance> SceneManager::instantiate_model( SharedPtr<Model>          model
                                                        , SharedPtr<ShaderProgram>  shader_program
                                                        , Transform const&          transform
                                                        , Bool                      has_tessellation_enabled
                                                        #ifdef   _DEBUG
                                                        , Bool                      is_a_debug_element
                                                        #endif /*DEBUG*/
                                                        )
{
   auto callback_lambda = [this]() {
                             _should_recalculate_shadowmap = true; // for lightmap recalculation
                             _should_sort_front_to_back    = true; // for front-to-back rendering
                          };
   // construct return value (shared pointer):
   auto instance_ptr = // TODO: switch to UniquePtr..?
      std::make_shared<ModelInstance>( model,
                                       shader_program,
                                       transform,
                                       callback_lambda,
                                       has_tessellation_enabled,
                                       _generate_light_id() );

   //if constexpr ( Config::is_debugging ) { // activate later when useful
   //   if ( is_a_debug_element ) {
   //      _debug_instances.push_back( instance_ptr );
   //      return instance_ptr;
   //   }
   //} /*DEBUG*/

   // add a weak pointer to the scene manager's instance list before returning:
   _instances.push_back( instance_ptr );
   return instance_ptr;
}

UniquePtr<Viewport> SceneManager::instantiate_viewport( Vec3                      position,
                                                        GLFWwindow               *window,
                                                        SharedPtr<ShaderProgram>  shader_program,
                                                        Float32                   fov_rad )
{
   auto callback_lambda = [this]() {
                             _should_sort_front_to_back = true; // for front-to-back rendering
                          };
   return std::make_unique<Viewport>( position, window, shader_program, callback_lambda, fov_rad );
}


Uint32 SceneManager::_generate_light_id() {
   return _next_light_id++;
}

Uint32 SceneManager::_generate_model_id() {
   return _next_model_id++;
}

void SceneManager::instantiate_particle_system( WeakPtr<ParticleSystem> ps ) { /* @TAG{PS} */
   _particle_systems.emplace_back( ps );
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

void SceneManager::update( Float32 delta_time_ms ) {
      for ( auto &ps : _particle_systems )
         if ( !ps.expired() )
            ps.lock()->update( delta_time_ms );
}

// TODO: use ShaderProgram::use()
void SceneManager::draw( Viewport &view ) {
   if ( _should_sort_front_to_back ) {
      _sort_by_distance( view ); // for front-to-back rendering
      _should_sort_front_to_back = false;
   }

   auto &g_buffer = view.get_g_buffer();

   auto lighting_pass_loc = _lighting_shader_program->get_location();
   auto geometry_pass_loc = _geometry_shader_program->get_location();

   // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // TODO: Make modelinstance supply unique ID to Callback Function and then in CAllback function compare the boundingbox of the modelinstance with the frustrum of all the active shadowcasters and recalculate shadowmap for any intersections
   if (_should_recalculate_shadowmap) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      this->update_shadowmap();
      _should_recalculate_shadowmap = false;
   }

   _tessellation_shader_program->use();
   /*CHANGE*/ view.bind_shader_program(_tessellation_shader_program);

   view.update();
   auto view_pos = view.get_view().get_position();
   glUniform3fv(glGetUniformLocation(_tessellation_shader_program->get_location(), "view_pos"),
      1,
      glm::value_ptr(view_pos));

  /*CHANGE*/ _geometry_shader_program->use();
  /*CHANGE*/ view.bind_shader_program( _geometry_shader_program );

   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, g_buffer.buffer_loc );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // toggle wireframe mode if config is set to true
   if ( config.is_wireframe_mode )
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

   // send view_pos to g_buffer.geom shader
   view.update();
   view_pos = view.get_view().get_position();
   glUniform3fv( glGetUniformLocation( geometry_pass_loc, "view_pos"),
                 1,
                 glm::value_ptr(view_pos));

   // 1. Geometry Pass:
   // TODO: sortera instanserna efter ShaderProgram m.h.a. std::partition()
   for ( auto &instance : _instances )
      if ( !instance.expired() )
         instance.lock()->draw();

   #ifdef _DEBUG
   if constexpr ( Config::is_debugging ) {
      if ( Config::should_draw_debug_elements ) {
         // for ( auto &instance : _debug_instances )
         //    if ( !instance.expired() )
         //        instance.lock()->draw();

         // draw light debug cues
         for ( auto &e : _lights ) {
            auto &light_ptr = std::get<1>( e );
            if ( !light_ptr.expired() )
               light_ptr.lock()->debug_circle.draw( view, _debug_line_shader );
         }
      }
   }
   #endif /*DEBUG*/

  // Particle system:

  // glBindFramebuffer(GL_FRAMEBUFFER,
  //    g_buffer.buffer_loc);
  //
  // glBindTexture(GL_TEXTURE_2D,
  //    g_buffer.alb_tex_loc);
  //
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  //
  // glBindTexture(GL_TEXTURE_2D,
  //    0);
  //
  // glFramebufferTexture2D(GL_FRAMEBUFFER,
  //    GL_COLOR_ATTACHMENT0,
  //    GL_TEXTURE_2D,
  //    g_buffer.alb_tex_loc,
  //    0);
  //
  // glDisable(GL_BLEND);

   //glBlendFunci(g_buffer.alb_tex_loc, GL_ZERO, GL_ONE);

   /*CHANGE*/ _particle_shader->use(); // gets called once per PS as well.. TODO: clean up
   /*CHANGE*/ view.bind_shader_program( _particle_shader );
   for ( auto &ps : _particle_systems )
      if ( !ps.expired() )
         ps.lock()->draw( view_pos, *_particle_shader );

   // disabling wireframe rendering so that the quad will render after the lighting pass
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

//@TAG{SSAO}
//*SSAO*/ glBindFramebuffer( GL_DRAW_FRAMEBUFFER, /*TODO*/ );
//*SSAO*/ glClear( GL_COLOR_BUFFER_BIT );
//*SSAO*/ SuperSampledAmbientOcclusion ssao; // TODO: wrap shader programs inside SSAO class?
//*SSAO*/ GBufferData gbuffer = view.get_g_buffer();
//*SSAO*/ glClear(         GL_COLOR_BUFFER_BIT );
//*SSAO*/ glActiveTexture( GL_TEXTURE0 );
//*SSAO*/ glBindTexture(   GL_TEXTURE_2D, gbuffer.nor_tex_loc );
//*SSAO*/ glActiveTexture( GL_TEXTURE1 );
//*SSAO*/ glBindTexture(   GL_TEXTURE_2D, gbuffer.pos_tex_loc );
//*SSAO*/ glActiveTexture( GL_TEXTURE2 );
//*SSAO*/ glBindTexture(   GL_TEXTURE_2D, ssao.get_noise_texture_location() );
//*SSAO*/ _ssao_main_shader->use();
//*SSAO*/ // TODO: upload resolution from view
//*SSAO*/ // TODO: upload ssao_kernel via uniform
//*SSAO*/ // TODO: upload projection matrix from view
//*SSAO*/ // TODO: unbind framebuffer
//*SSAO*/ // TODO STUFF
//*SSAO*/ _ssao_blur_shader->use();
//*SSAO*/ // TODO: use in lightshad.frag

// 2. Lighting pass:
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
   // TODO: refactor lighting pass code here

   glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   _lighting_shader_program->use(); // glUseProgram( lighting_pass_loc );

   auto g_buffer_data { view.get_g_buffer() };

// @TAG{TEXTURE_CHANNEL}
   glActiveTexture( GL_TEXTURE0) ;
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.alb_tex_loc );

   glActiveTexture( GL_TEXTURE1 );
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.spe_tex_loc );

   glActiveTexture( GL_TEXTURE2 );
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.nor_tex_loc );

   glActiveTexture( GL_TEXTURE3) ;
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.emi_tex_loc );

   glActiveTexture( GL_TEXTURE4 );
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.dis_tex_loc );

   glActiveTexture( GL_TEXTURE5 );
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.pos_tex_loc );

   glActiveTexture( GL_TEXTURE6 );
   glBindTexture(   GL_TEXTURE_2D, g_buffer_data.pic_tex_loc );

   glUniform3fv( glGetUniformLocation( lighting_pass_loc, "view_pos"),
                 1,
                 glm::value_ptr(view_pos));

   for ( auto &e : _shadow_maps ) {
      glUniformMatrix4fv(
         glGetUniformLocation(lighting_pass_loc,
            "lightmatrix"),
         1,
         GL_FALSE,
         glm::value_ptr( e.first->get_matrix()) );
      glActiveTexture( GL_TEXTURE7 );
      glBindTexture( GL_TEXTURE_2D, e.second );
   }


   _lights_to_gpu();
   _render_to_quad();
}


void SceneManager::_initialize_quad_vao() {
   static Bool has_run = false;

   if ( has_run )
      return; // early exit

   Float32 quad_verts[] = { // NOTE: try setting as static in case of error?
      //   X      Y     Z       U     V
         -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
         -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
          1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
          1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
   };

   // setup plane VAO
   glGenVertexArrays( 1, &_quad_vao );
   glGenBuffers(      1, &_quad_vbo );

   glBindVertexArray( _quad_vao );
   glBindBuffer( GL_ARRAY_BUFFER, _quad_vbo );

   glBufferData( GL_ARRAY_BUFFER,
                 sizeof(quad_verts),
                 &quad_verts,
                 GL_STATIC_DRAW);

   glEnableVertexAttribArray(0);

   // screen 2D position
   glVertexAttribPointer( 0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(Float32), // X,Y,Z,U,V = 5 Float32 channels
                          (void*)0);

   glEnableVertexAttribArray(1);

   // screen 2D UV
   glVertexAttribPointer( 1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(Float32),
                          (void*)(3 * sizeof(Float32)));
}



void SceneManager::_render_to_quad() {
   glBindVertexArray( _quad_vao );
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
            transform.set_rotation( Vec3(1.0f, 0.0f, 0.0f), rotation.x );
            transform.set_rotation( Vec3(0.0f, 1.0f, 0.0f), rotation.y );
            transform.set_rotation( Vec3(0.0f, 0.0f, 1.0f), rotation.z );

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



void SceneManager::set_shadowcasting( SharedPtr<Shadowcaster> light ) {
   //TODO: If-statement to check validity of light, is it directional?
   use_depth_map_FBO();

   Uint32 depthMap, width, height;
   glGenTextures(1, &depthMap);

   width  = config.shadowmap_side;
   height = config.shadowmap_side; //TODO: Enable SetSize of Width and height

   glBindTexture(GL_TEXTURE_2D, depthMap);

   glTexImage2D( GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT,
                 width,
                 height,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 NULL);

   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR );

   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR );

   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_BORDER);

   glTexParameteri( GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_BORDER);

//   glTexParameteri(GL_TEXTURE_2D,
//      GL_CLAMP_TO_BORDER, //  LOWE!
//      GL_CLAMP_TO_BORDER);

   float borderColor[] = { 0.0,0.0,0.0, 0.0 };
   glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );

   //Uint32 AttatchmentNmbr = _shadow_maps.size();

   glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0 );
   glDrawBuffer( GL_NONE );
   glReadBuffer( GL_NONE );

   _shadow_maps.emplace( light, depthMap );
   glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void SceneManager::_init_depth_map_FBO()
{
   glGenFramebuffers( 1, &_depth_map_FBO_id );
}

void SceneManager::use_depth_map_FBO()
{
   glBindFramebuffer( GL_FRAMEBUFFER, _depth_map_FBO_id );
}

void SceneManager::update_shadowmap()
{
   if ( _shadow_maps.size() == 0 )
      return;

   //glBindFramebuffer(GL_FRAMEBUFFER, this->_depth_map_FBO_id);
   use_depth_map_FBO();
   glUseProgram(_shadow_depth_shader->get_location());

   GLuint count = 0;
   GLint oldSize[4];
   glGetIntegerv(GL_VIEWPORT, oldSize);


   for ( auto &e : _shadow_maps ) {
      //_shadowcasters[i]
      //send lightMatrix to
      glUniformMatrix4fv(
         glGetUniformLocation(_shadow_depth_shader->get_location(),
            "lightmatrix"),
         1,
         GL_FALSE,
         glm::value_ptr(e.first->get_matrix()));

      glViewport(0, 0, config.shadowmap_side, config.shadowmap_side); // TODO: do not hardcode, decide based on Shadowcaster


      glClear(GL_DEPTH_BUFFER_BIT);

      //TODO: Increment GL_TEXTURE
      //glActiveTexture(GL_TEXTURE0/*+ count*/);

      //Render from light pov
      for ( auto &instance : _instances ) { //TODO: Fix Shadowcasting for tessellated models
         if ( !instance.expired() ) {
            auto model    = instance.lock();
            auto previous = model->get_shader_program();
            auto is_tessellated = model->get_is_tessellation_enabled();
            if ( is_tessellated )
               model->set_is_tessellation_enabled( false );
            model->set_shader_program(this->_shadow_depth_shader);
            model->draw();
            model->set_shader_program(previous);
            if ( is_tessellated )
               model->set_is_tessellation_enabled( true );
         }
      }

      count++;
   }
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glViewport(0, 0, oldSize[2], oldSize[3]);

}

SceneManager::SceneManager( SharedPtr<ShaderProgram> geo_pass
                          , SharedPtr<ShaderProgram> geo_pass_tessellated
                          , SharedPtr<ShaderProgram> light_pass
                          , SharedPtr<ShaderProgram> shadow_depth
                          , SharedPtr<ShaderProgram> particle_shader   /* @TAG{PS} */
                       #ifdef _DEBUG
                          , SharedPtr<ShaderProgram> dbg_line_program
                       #endif /*DEBUG*/
               //* SSAO */, SharedPtr<ShaderProgram> ssao_main_shader
               //* SSAO */, SharedPtr<ShaderProgram> ssao_blur_shader
                          )
:
         _geometry_shader_program      ( geo_pass             ),
         _tessellation_shader_program  ( geo_pass_tessellated ),
         _lighting_shader_program      ( light_pass           ),
         _shadow_depth_shader          ( shadow_depth         ),
         _particle_shader              ( particle_shader      ),  /* @TAG{PS} */
     #ifdef _DEBUG
         _debug_line_shader            ( dbg_line_program     ),
     #endif /*DEBUG*/
//*SSAO*/_ssao_main_shader             ( ssao_main_shader     ),
//*SSAO*/_ssao_blur_shader             ( ssao_blur_shader     ),
         _num_lights                   ( 0                    )
{
   _initialize_quad_vao();
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

// returns the ID of the fragment at XY (or 0 if no model instances occupy it)
//
//
// @arg x     screen X coordinate
// @arg y     screen Y coordinate
// @arg view  viewport (to access G-buffer mouse picking channel)
Uint32 SceneManager::get_object_id_at_pixel( Uint32 x, Uint32 y, Viewport &view ) const {
   // we bind the mouse picking channel from the G-buffer
   glBindFramebuffer( GL_READ_FRAMEBUFFER, view.get_g_buffer().buffer_loc );
   glReadBuffer(      GL_COLOR_ATTACHMENT6 );

   // struct to contain the data glReadPixels return
   struct pixel_info_struct { Float32 x;
                              Float32 y;
                              Float32 z;
                              Float32 w; };

   pixel_info_struct pixel_info;

   glReadPixels( x,             // the X-coord is unaffected
                 view.height-y, // map the Y-coord to conform with openGL (top 0 instead of 1)
                 1,             // width  (in pixels) of rectangle to read; 1x1 pixels in our case
                 1,             // height (in pixels) of rectangle to read; 1x1 pixels in our case
                 GL_RGBA,       // read pixels as RGBA
                 GL_FLOAT,      // store as floats
                 &pixel_info ); // pointer to first buffer element (1 pixel in our case)


   // probably don't need the & 0xFF since we send up in increments of 8 bytes
   Uint32 obj_id  = ( ((Int32)(pixel_info.x * 255) & 0xFF) <<  0)
                  + ( ((Int32)(pixel_info.y * 255) & 0xFF) <<  8)
                  + ( ((Int32)(pixel_info.z * 255) & 0xFF) << 16)
                  + ( ((Int32)(pixel_info.w * 255) & 0xFF) << 24);

   // clean-up
   glReadBuffer( GL_NONE ); // TODO: change bind clearing zeroes to GL_NONE
   glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );

   return obj_id;
}

SharedPtr<ModelInstance> SceneManager::get_instance_ptr( Uint32 obj_id ) {
   for ( auto &e : _instances ) {
      if ( !e.expired() ) {
         auto e_ptr = e.lock();
         if ( e_ptr->id == obj_id )
            return e_ptr;
      }
   }

   if constexpr ( Config::is_debugging )
      std::cout << "[MOUSE_PICKING] no model hit\n";

   return nullptr;
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




void SceneManager::_sort_by_distance( Viewport const &viewport ) {
   auto const &cam_pos = viewport.get_view().get_position();
   std::sort( _instances.begin(),
              _instances.end(),
              [&cam_pos]( WeakPtr<ModelInstance> const &lhs, WeakPtr<ModelInstance> const &rhs ) { // comparator lambda
                    // extract the two elements' potential distances:
                    auto lhs_dist = lhs.expired() ? std::numeric_limits<float>::infinity()
                                                  : glm::distance( cam_pos, lhs.lock()->model_transform.get_position() );
                    auto rhs_dist = rhs.expired() ? std::numeric_limits<float>::infinity()
                                                  : glm::distance( cam_pos, rhs.lock()->model_transform.get_position() );
                    // compare the two distances:
                    return lhs_dist < rhs_dist;
                 } );
}
