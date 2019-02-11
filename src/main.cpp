#define STB_IMAGE_IMPLEMENTATION //Den bor här annars kompilerar inte stb_image.h

#include "misc/defs.h"
#include "Config.h"

#include "misc/ImGui/imgui.h"
#include "misc/ImGui/imgui_impl_glfw.h"
#include "misc/ImGui/imgui_impl_opengl3.h"

#include "AssetManager.h"
#include "ShaderManager.h"
#include "Viewport.h"
#include "SceneManager.h"

// #include "misc/stb_image.h"
// #include <range/v3/all.hpp>

Float32 g_move_speed            = 25.0f; // TODO: refactor away ugly globalness
Bool    g_is_mouse_look_enabled = false;

void create_demo_scene( /*...*/ ) {
   // TODO: setup the viewport camera

   // load the statue into memory
   //auto statue_handle = assets.load_model( "12330_Statue_v1_L2.obj" );

   // create 9 statues arranged thusly:
   //   0  1  2
   //   3  4  5
   //   6  7  8
   for ( int i=0;  i<9;  ++i ) {
      glm::vec3 position { i/3, i%3, 0 };
      //auto instance_id = scene.add_object( statue_handle, position );
   }
}




[[nodiscard]] String lowercase( char const *base ) {
   String s ( base );
   std::transform( s.begin(), s.end(), s.begin(), ::tolower );
   return s;
}

[[nodiscard]] String uppercase( char const *base ) {
   String s ( base );
   std::transform( s.begin(), s.end(), s.begin(), ::toupper );
   return s;
}




void process_mouse( GLFWwindow *window, Viewport &cam, Float32 delta_time_s  ) {
   // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a
   // direction vector pointing to the right so we initially rotate a bit to the left.
   static Float64 yaw         = -90.0f;
   static Float64 pitch       =  0.0f;
   static Float64 last_x      =  800.0f / 2.0;
   static Float64 last_y      =  600.0 / 2.0;
   static Bool    first_mouse = true;

   Float64 x_pos, y_pos;

   if ( !g_is_mouse_look_enabled )
      return;

   glfwGetCursorPos( window, &x_pos, &y_pos );

   if ( first_mouse ) {
      last_x = x_pos;
      last_y = y_pos;
      first_mouse = false;
   }

   bool changed = last_x != x_pos || last_y != y_pos;

   Float64 x_offset = x_pos - last_x;
   Float64 y_offset = last_y - y_pos;
   last_x = x_pos;
   last_y = y_pos;

   Float64 sensitivity = 0.05;
   x_offset *= sensitivity;
   y_offset *= sensitivity;

   yaw   += x_offset;
   pitch += y_offset;

   if ( pitch > 89.0f )
      pitch =  89.0f;
   if ( pitch < -89.0f )
      pitch = -89.0f;

   if ( changed ) {
      cam.forward.x = cos( glm::radians(yaw)) * cos(glm::radians(pitch) );
      cam.forward.y = sin( glm::radians(pitch)                          );
      cam.forward.z = sin( glm::radians(yaw)) * cos(glm::radians(pitch) );
      glm::normalize( cam.forward );
      auto view = cam.get_view();  // get view (pos, rot, scale)
      view.look_at( cam.forward ); // rotate view
      cam.set_view( view );        // update cam view
   }
}




void process_input( GLFWwindow *window, Viewport &cam, Float32 time_delta_s ) {
   glfwSetInputMode( window, GLFW_STICKY_KEYS, 1 );

   if ( glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS )
      glfwSetWindowShouldClose(window, true);

   if ( glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS ) {
      g_is_mouse_look_enabled = !g_is_mouse_look_enabled;
      if ( g_is_mouse_look_enabled )
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      else
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   }

   if ( glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS )
      config.is_wireframe_mode = !config.is_wireframe_mode; // used in SceneManager::Draw()

   if ( glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS )
      config.render_mode = RenderMode::composite;

   if ( glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS )
      config.render_mode = RenderMode::albedo;

   if ( glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS )
      config.render_mode = RenderMode::normals;

   if ( glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS )
      config.render_mode = RenderMode::specular;

   if ( glfwGetKey(window, GLFW_KEY_F7) == GLFW_PRESS )
      config.render_mode = RenderMode::positional;

//   if ( glfwGetKey(window, GLFW_KEY_F8) == GLFW_PRESS )
//      config.render_mode = RenderMode::emission; // TODO!



   Float32 move_distance = g_move_speed * time_delta_s;
   Transform offset;

   if ( glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(1.0, 1.0, move_distance )* -cam.forward);
      cam.transform( offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(-1.0, -1.0, -move_distance)* -cam.forward);
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(move_distance, 1.0, 1.0)* glm::cross( cam.forward, Vec3(0.0, 1.0f, 0.0f)));
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(-move_distance, -1.0, -1.0)* glm::cross( cam.forward, Vec3(0.0, 1.0f, 0.0f)));
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(0.0, move_distance, 0.0) );
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(0.0, -move_distance, 0.0));
      cam.transform(offset);
   }

/*
   if ( glfwGetKey(window, GLFW_KEY_W)            == GLFW_PRESS ) { // forward
      cam.transform( translation * forward );
   }
   if ( glfwGetKey(window, GLFW_KEY_S)            == GLFW_PRESS ) { // backward
      // auto backward = forward * Transform::make_rotation( Vec3(   0,  180,  0 ) );
      // cam.transform( translation * backward );
   }
   if ( glfwGetKey(window, GLFW_KEY_A)            == GLFW_PRESS ) { // left
      // auto left     = forward * Transform::make_rotation( Vec3(   0,  -90,  0 ) );
      // cam.transform( translation * left );
   }
   if ( glfwGetKey(window, GLFW_KEY_D)            == GLFW_PRESS ) { // right
      // auto right    = forward * Transform::make_rotation( Vec3(   0,   90,  0 ) );
      // cam.transform( translation * right );
   }
   if ( glfwGetKey(window, GLFW_KEY_SPACE)        == GLFW_PRESS ) { // up
      // auto up       = forward * Transform::make_rotation( Vec3( -90,    0,  0 ) );
      // cam.transform( translation * up );
   }
   if ( glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ) { // down
      // auto down     = forward * Transform::make_rotation( Vec3( -90,    0,  0 ) );
      // cam.transform( translation * down );
   }
*/
}




/*
void process_input( GLFWwindow *window, Viewport &cam, Float32 delta ) {
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
   if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
      mouse_look = !mouse_look;
      if (mouse_look)
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      else
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   }

   //If movement is disabled
   if ( !mouse_look )
      return;

   // TODO: fix camera navigation
   Float32 camspeed = g_move_speed * delta;
   Transform offset;


   if ( glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(1.0, 1.0, camspeed )* -cam.front);
      cam.transform( offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(-1.0, -1.0, -camspeed)* -cam.front);
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(camspeed , 1.0, 1.0)*glm::cross(cam.front, Vec3(0.0, 1.0f, 0.0f)));
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(-camspeed, -1.0, -1.0)* glm::cross( cam.front, Vec3(0.0, 1.0f, 0.0f)));
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(0.0, camspeed, 0.0) );
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ) {
      offset = Transform::make_translation(Vec3(0.0, -camspeed, 0.0));
      cam.transform(offset);
   }
}
*/




void draw_camera_debug_window( Vec3    &position,
                               Vec3    &rotation,
                               Float32 &fov_rad ) {
   ImGui::Begin( "Camera:" ); // begin our Camera window:
   {  // draw our window GUI components and do I/O:
      ImGui::SliderAngle( "X-axis", &rotation.x );
      ImGui::SliderAngle( "Y-axis", &rotation.y );
      ImGui::SliderAngle( "Z-axis", &rotation.z );
      // add some vertical spacing:
      ImGui::Spacing();
      // read in the camera position:
      ImGui::SliderFloat( "X pos", &position.x, -255, +255 );
      ImGui::SliderFloat( "Y pos", &position.y, -255, +255 );
      ImGui::SliderFloat( "Z pos", &position.z, -255, +255 );
      // add some vertical spacing:
      ImGui::Spacing();
      // FOV slider:
      ImGui::SliderAngle( "FOV", &fov_rad );
      // add some vertical spacing:
      ImGui::Spacing();
      // print our current framerate:
      ImGui::Text( "\t%.1f FPS (avg %.3f ms/frame)",
                  ImGui::GetIO().Framerate,
                  1000.0f / ImGui::GetIO().Framerate );
   } ImGui::End(); // end our Camera window
}




Int32 main( Int32 argc, char const *argv[] ) {
	// initialise GLFW
	glewExperimental = true; // <- needed for core profile
	if (!glfwInit()) {
		fprintf( stderr, "[ERROR] Failed to initialize GLFW.\n" );
		return -1;
	}

	// 4xAA
	glfwWindowHint( GLFW_SAMPLES, 4 );
	// GLSL v130
	char const *glsl_version = "#version 440";
	// OpenGL v4.4
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 4 );
	// for MacOS; should not be needed
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	// use OpenGL core profile
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	// open a window and create its OpenGL context
	GLFWwindow *window;

	window = glfwCreateWindow( Config::start_width,
                              Config::start_height,
                              "3D Project -- WINDOW",
                              NULL,
                              NULL );

	if ( window == NULL ) {
		fprintf(stderr, "[ERROR] Failed to open GLFW window.\n"
			             "        If you have an Intel GPU, they're not 4.4 compatible.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent( window ); // initialize GLEW

   // glfwSwapInterval(1); // enable vsync -- TODO: decide!

	if ( glewInit() != GLEW_OK ) {
		fprintf( stderr, "[ERROR] Failed to initialize GLEW.\n" );
		return -1;
	}

	// ensure we can capture the escape key being pressed below
	glfwSetInputMode( window, GLFW_STICKY_KEYS, 1 );
   // glfwSetCursorPosCallback(window, mouse_callback); // TODO: make a call back matching template that calls on our process_mouse();
   glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   // glfwSetCursorPosCallback(window, process_mouse);

	// ImGui context setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io; // wot?
 // io.configFlags |= ImGuiconfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
 // io.configFlags |= ImGuiconfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// setup ImGui style
	ImGui::StyleColorsDark();

	// setup platform and renderer bindings
	ImGui_ImplGlfw_InitForOpenGL( window, true );
	ImGui_ImplOpenGL3_Init( glsl_version );

	glEnable( GL_DEPTH_TEST   );
	glEnable( GL_STENCIL_TEST );

   ShaderManager shader_manager {};
   AssetManager  asset_manager  {};
   SceneManager  scene_manager  {};

   auto lighting_vert_shader  { shader_manager.load_shader( "lightSha.vert" ) }; // TODO: rename files
   auto lighting_frag_shader  { shader_manager.load_shader( "lightSha.frag" ) }; // TODO: rename files
   auto geometry_vert_shader  { shader_manager.load_shader( "g_buffer.vert" ) }; // TODO: rename files
   auto geometry_frag_shader  { shader_manager.load_shader( "g_buffer.frag" ) }; // TODO: rename files
   auto geometry_program      { shader_manager.create_program({ geometry_frag_shader, geometry_vert_shader }) };
   auto lighting_program      { shader_manager.create_program({ lighting_frag_shader, lighting_vert_shader }) };

   SharedPtr<Model> nanosuit_model = asset_manager.load_model( "nanosuit.obj" );

   Vector<SharedPtr<ModelInstance>> model_instances;
   model_instances.reserve( 64 );
   for ( auto i=0;  i<64;  ++i ) {
      Float32 n = 9; // spacing
      model_instances.push_back(
         scene_manager.instantiate_model( nanosuit_model,
                                          geometry_program,
                                          Transform( Vec3( n*(i/8)-40,  0.0f,  n*(i%8)-40 ),
                                                     Vec3(       0.0f,  0.0f,        0.0f ),
                                                     Vec3(       1.3f,  1.3f,        1.3f ) ) ) );
   }

   /* TODO */ Vec3       cam_rotations {  0.0f,   0.0f,   0.0f };
   /* TODO */ Vec3       cam_position  {  0.0f, -20.0f,  15.0f };
   /* TODO */ Transform  cam_transform;
   /* TODO */ Float32    fov_rad { Config::fov_rad }; // 90 degrees
   /* TODO */ Viewport view { cam_position, window, fov_rad };
   /* TODO */ view.bind_shader_program( *geometry_program );
   /* TODO */ //TODO: remove when we dont want to se dogass
   /* TODO */ Transform rotate_deg180 = view.get_view();
   /* TODO */ rotate_deg180.rotate_deg( Vec3( 0.0f, 1.0f, 0.0f ), 180.0f );
   /* TODO */ view.set_view( rotate_deg180 );
   /* TODO */ view._g_buffer_init();
   /* TODO */ //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   glUseProgram( lighting_program->get_location() );

   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_pos"    ), 0 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_norm"   ), 1 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_spec"   ), 2 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_albedo" ), 3 );
   // TODO: emission map

   Uint32 const num_lights { 8 };
   glUniform1i( lighting_program->get_location(), num_lights );

   LightData lights[light_capacity];

   lights[0] = LightData{ LightType::point,
                          Vec3(  0.0f,   0.0f,   0.0f ),
                          Vec3( 10.0f,  10.0f,  10.0f ),
                          Vec3(  1.0f,   0.0f,   0.0f ),
                           1.0,
                          14.0,
                           0.0,
                           1.0 };

   lights[1] = LightData{ LightType::point,
                          Vec3( 0.0f,  0.0f,  0.0f ),
                          Vec3( 1.0f,  4.0f,  5.0f ),
                          Vec3( 1.0f,  1.0f,  0.0f ),
                          1.0,
                          7.0,
                          0.0,
                          1.0 };

   lights[2] = LightData{ LightType::point,
                          Vec3( 0.0f,  0.0f,  0.0f ),
                          Vec3( 2.0f,  1.0f,  5.0f ),
                          Vec3( 1.0f,  0.0f,  1.0f ),
                           1.0,
                          17.0,
                           0.0,
                           1.0 };

   lights[3] = LightData{ LightType::point,
                          Vec3( 0.0f,  0.0f,  0.0f ),
                          Vec3( 1.0f,  5.0f,  6.0f ),
                          Vec3( 0.0f,  1.0f,  0.0f ),
                           1.0,
                          11.0,
                           0.0,
                           1.0 };

   lights[4] = LightData{ LightType::point,
                          Vec3( 0.0f,  0.0f,  1.0f ),
                          Vec3( 3.0f,  3.0f,  1.0f ),
                          Vec3( 0.0f,  1.0f,  1.0f ),
                          1.0,
                          2.0,
                          0.0,
                          1.0 };

   lights[5] = LightData{ LightType::point,
                          Vec3( 0.0f,  0.0f,   0.0f ),
                          Vec3( 1.0f,  2.0f,  10.0f ),
                          Vec3( 0.0f,  0.0f,   1.0f ),
                          1.0,
                          1.0,
                          0.0,
                          1.0 };

   lights[6] = LightData{ LightType::point,
                          Vec3(  0.0f,  0.0f,  0.0f ),
                          Vec3( 10.0f,  0.0f,  5.0f ),
                          Vec3(  1.0f,  1.0f,  1.0f ),
                          1.0,
                          7.0,
                          0.0,
                          1.0 };

   lights[7] = LightData{ LightType::point,
                          Vec3(   0.0f,  0.0f,   0.0f ),
                          Vec3(  10.0f,  5.0f,  10.0f ),
                          Vec3(   1.0f,  0.3f,   0.5f ),
                           1.0,
                          17.0,
                           0.0,
                           1.0 };

   Uint32  quad_vao = 0;
   Uint32  quad_vbo;

// glDisable( GL_BLEND );
// main loop:
	while ( !glfwWindowShouldClose(window) ) {
      Float32 delta_time_s { ImGui::GetIO().DeltaTime };

		// poll & handle events such as window resizing and input from the keyboard or mouse
		// use io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if imgui wants to use the user's input
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

      // myView.bind_shader_program( *shaProg );
      // draw_camera_debug_window( cam_position, cam_rotations, fov_rad );
      // cam_transform.set_rotation( cam_rotations );
      // cam_transform.set_position( cam_position );
      // myView.set_view( cam_transform );
      // myView.set_fov( fov_rad );
      scene_manager.draw_debug_scene_inspection();
      ImGui::Begin( "Settings:" );
      ImGui::SliderFloat( "Move speed", &g_move_speed, 0.0f, 25.0f );
      ImGui::End();

      process_mouse( window, view, delta_time_s );
      process_input( window, view, delta_time_s );

      // glMatrixMode( GL_PROJECTION );
      // glLoadIdentity();

      glUseProgram( geometry_program->get_location() );
      // TODO: we bind the shader program to the glUseProgram before sending the view_pos...
      view.update();
      auto view_pos = view.get_view().get_position();

      scene_manager.draw( view );

      // glUseProgram(shaProg->get_location());
      // a_Mesh.render();
      auto g_buffer_data { view.get_g_buffer() };

      // glUseProgram( lightProg->get_location() );
      glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); // ( 0.4f, 0.6, 1.0, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      glUseProgram( lighting_program->get_location() );

      glActiveTexture( GL_TEXTURE0 );
      glBindTexture(   GL_TEXTURE_2D, g_buffer_data.pos_tex_loc );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture(   GL_TEXTURE_2D, g_buffer_data.nor_tex_loc );
      glActiveTexture( GL_TEXTURE2 );
      glBindTexture(   GL_TEXTURE_2D, g_buffer_data.spe_tex_loc );
      glActiveTexture( GL_TEXTURE3 );
      glBindTexture(   GL_TEXTURE_2D, g_buffer_data.alb_tex_loc ); //TODO:P Denna är den enda som gör något...

      for ( Uint32 i = 0;  i < num_lights;  ++i ) { //TODO:P Endast en ljuskälla ger samma resultat som att använda alla...
         LightData &ld = lights[i];
         glUniform1ui( glGetUniformLocation(lighting_program->get_location(), ("lights[" + std::to_string(i) + "].type").c_str()),        ld.type);
         glUniform3fv( glGetUniformLocation(lighting_program->get_location(), ("lights[" + std::to_string(i) + "].dir").c_str()),         1, glm::value_ptr(ld.direction));
         glUniform3fv( glGetUniformLocation(lighting_program->get_location(), ("lights[" + std::to_string(i) + "].pos").c_str()),         1, glm::value_ptr(ld.position));
         glUniform3fv( glGetUniformLocation(lighting_program->get_location(), ("lights[" + std::to_string(i) + "].rgb").c_str()),         1, glm::value_ptr(ld.color));
         glUniform1f(  glGetUniformLocation(lighting_program->get_location(), ("lights[" + std::to_string(i) + "].intensity").c_str()),   ld.intensity);
         glUniform1f(  glGetUniformLocation(lighting_program->get_location(), ("lights[" + std::to_string(i) + "].radius").c_str()),      ld.radius);
         glUniform1f(  glGetUniformLocation(lighting_program->get_location(), ("lights[" + std::to_string(i) + "].degree").c_str()),      ld.degree);
         glUniform1f(  glGetUniformLocation(lighting_program->get_location(), ("lights[" + std::to_string(i) + "].specularity").c_str()), ld.specularity );
      }
      glUniform1ui( glGetUniformLocation(lighting_program->get_location(), "num_lights"),  num_lights );
      glUniform3fv( glGetUniformLocation(lighting_program->get_location(), "view_pos"), 1, glm::value_ptr(view_pos));
      glUniform1ui( glGetUniformLocation(lighting_program->get_location(), "render_mode"), (Uint32)config.render_mode );

      if ( 0 == quad_vao ) {
         Float32 quad_verts[] = {
         //   X      Y     Z       U     V
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
         };

         // setup plane VAO
         glGenVertexArrays( 1, &quad_vao );
         glGenBuffers(      1, &quad_vbo );
         glBindVertexArray(     quad_vao );

         glBindBuffer( GL_ARRAY_BUFFER, quad_vbo );

         glBufferData( GL_ARRAY_BUFFER,
                       sizeof(quad_verts),
                       &quad_verts,
                       GL_STATIC_DRAW );

         glEnableVertexAttribArray( 0 );

         glVertexAttribPointer( 0,
                                3,
                                GL_FLOAT,
                                GL_FALSE,
                                5 * sizeof(Float32),
                                (void*)0 );

         glEnableVertexAttribArray( 1 );

         glVertexAttribPointer( 1,
                                2,
                                GL_FLOAT,
                                GL_FALSE,
                                5 * sizeof(Float32),
                                (void*)(3 * sizeof(Float32)) );
      }
      glBindVertexArray( quad_vao );

      glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

      glBindVertexArray( 0 );

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
      glfwMakeContextCurrent( window );
      glfwSwapBuffers( window );
	} // main loop end


   // cleanup:
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow( window );
	glfwTerminate(); // close OpenGL window & terminate GLFW
	return 0; // successful exit
}
