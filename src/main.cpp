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

void process_input(GLFWwindow *window, Viewport &, Float32);
void process_mouse(GLFWwindow *window, Viewport &cam, Float32 delta);

// #include "misc/stb_image.h"
//
// #include <range/v3/all.hpp>


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
   String s( base );
   std::transform( s.begin(), s.end(), s.begin(), ::tolower );
   return s;
}

[[nodiscard]] String uppercase( char const *base ) {
   String s = base;
   std::transform( s.begin(), s.end(), s.begin(), ::toupper );
   return s;   
}




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



static bool mouse_look = false;
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

   //TODO: handle Window Resize!
	window = glfwCreateWindow( config::width, config::height, "3D Project -- WINDOW", NULL, NULL );
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
	glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );
   //glfwSetCursorPosCallback(window, mouse_callback); // TODO: make a call back matching template that calls on our process_mouse();
   glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

   
   // glfwSetCursorPosCallback(window, process_mouse);

	// ImGui context setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io; // wot?
 // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
 // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// setup ImGui style
	ImGui::StyleColorsDark();

	// setup platform and renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// activate depth test and stencil test for openGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

   //testa   vår AssetManager TODO: heh 
   ShaderManager shaMan{};
   auto fraShader = shaMan.load_shader("fraSha.frag");
   auto vertShader = shaMan.load_shader("vertSha.vert");
   auto light_f_Shader = shaMan.load_shader("lightSha.frag");
   auto light_v_Shader = shaMan.load_shader("lightSha.vert");
   auto geo_f_Shader = shaMan.load_shader("g_buffer.frag");
   auto geo_v_Shader = shaMan.load_shader("g_buffer.vert");

   auto quad_FShader = shaMan.load_shader("quad.frag");
   auto quad_VShader = shaMan.load_shader("quad.vert");

   auto shaProg = shaMan.create_program({fraShader, vertShader});
   auto geoProg = shaMan.create_program({ geo_f_Shader, geo_v_Shader });
   auto lightProg = shaMan.create_program({ light_f_Shader, light_v_Shader });
   auto quadProg = shaMan.create_program({ quad_FShader, quad_VShader });

   AssetManager assMan{};
   SharedPtr<Model> myModel2 = assMan.load_model("nanosuit.obj");
   SharedPtr<Model> myModel  = assMan.load_model("Dog.dae");
   
   
   SceneManager scenMan{};


   SharedPtr<ModelInstance> modelInstance2 =
      scenMan.instantiate_model(myModel2,
         shaProg,
         Transform(Vec3(0.0f, 6.0f, 3.0f), Vec3(0.0f), Vec3(1.3f, 1.3f, 1.3f)));

   SharedPtr<ModelInstance> modelInstance = 
      scenMan.instantiate_model( myModel,
         shaProg,
                                 Transform(Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f), Vec3(14.0f, 14.0f, 14.0f)));


   //scenMan

   Vec3 cam_rotations { .0f, .0f,  .0f };
   Vec3 cam_position  { 0.0f, -20.0f, 15.0f };
   Transform cam_transform;
   Float32 fov_rad = config::fov_rad; // 90 degrees
   Viewport myView { cam_position, window, fov_rad };
   myView.bind_shader_program(*shaProg);
   
   //TODO: remove when we dont want to se dogass
   Transform rotate_deg180 = myView.get_view();
   rotate_deg180.rotate_deg(Vec3(0.0, 1.0, 0.0), 180.f);
   myView.set_view(rotate_deg180);
   myView._g_buffer_init();
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   

   glUseProgram( lightProg->getProgramLoc() );
   
   //TODO:P Bör skicka in riktiga värden så att färgen fungerar...?
   glUniform1i( glGetUniformLocation(lightProg->getProgramLoc(),"g_tex_pos"), 0);
   glUniform1i( glGetUniformLocation(lightProg->getProgramLoc(),"g_tex_norm"), 1);
   glUniform1i( glGetUniformLocation(lightProg->getProgramLoc(),"g_tex_spec"), 2);
   glUniform1i( glGetUniformLocation(lightProg->getProgramLoc(),"g_tex_albedo"), 3); //TODO:P Denna är den enda som används...
   
   int const numlight = 8;
   glUniform1i(lightProg->getProgramLoc(), numlight);

   LightData lights[numlight];

   lights[0] = LightData{ LightType::point, 
                           Vec3(0.0f), 
                           Vec3(10.f,10.f,10.f),
                           Vec3(1.0f,0.f,0.f), 
                           1.0,
                           14.0, 
                           0.0,
                           1.0};
   lights[1] = LightData{ LightType::point,
                           Vec3(0.0f),
                           Vec3(1.f,4.f,5.f),
                           Vec3(1.0f,1.f,0.f),
                           1.0,
                           7.0,
                           0.0,
                           1.0 };
   lights[2] = LightData{ LightType::point,
                           Vec3(0.0f),
                           Vec3(2.f,1.f,5.f),
                           Vec3(1.0f,0.f,1.f),
                           1.0,
                           17.0,
                           0.0,
                           1.0 };
   lights[3] = LightData{ LightType::point,
                           Vec3(0.0f),
                           Vec3(1.f,5.f,6.f),
                           Vec3(.0f,1.f,0.f),
                           1.0,
                           11.0,
                           0.0,
                           1.0 };
   lights[4] = LightData{ LightType::point,
                           Vec3(0.0f),
                           Vec3(3.f,3.f,1.f),
                           Vec3(.0f,1.f,1.f),
                           1.0,
                           2.0,
                           0.0,
                           1.0 };
   lights[5] = LightData{ LightType::point,
                           Vec3(0.0f),
                           Vec3(1.f,2.f,10.f),
                           Vec3(.0f,0.f,1.f),
                           1.0,
                           1.0,
                           0.0,
                           1.0 };
   lights[6] = LightData{ LightType::point,
                           Vec3(0.0f),
                           Vec3(10.f,0.f,5.f),
                           Vec3(1.0f,1.f,1.f),
                           1.0,
                           7.0,
                           0.0,
                           1.0 };
   lights[7] = LightData{ LightType::point,
                           Vec3(0.0f),
                           Vec3(10.f,5.f,10.f),
                           Vec3(1.0f,0.3f,0.5f),
                           1.0,
                           17.0,
                           0.0,
                           1.0 };
   
   
   

   unsigned int quad_vao = 0;
   unsigned int quad_vbo;
   
   //glDisable(GL_BLEND);
 // main loop:
	while (!glfwWindowShouldClose(window)) {
      Float32 delta_time_s = ImGui::GetIO().DeltaTime; 

      
    
      

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

//myView.bind_shader_program(*shaProg);
      // draw_camera_debug_window( cam_position, cam_rotations, fov_rad );
      // cam_transform.set_rotation( cam_rotations );
      // cam_transform.set_position( cam_position );
      // myView.set_view( cam_transform );
      // myView.set_fov( fov_rad );
      scenMan.draw_debug_scene_inspection();


      process_mouse( window, myView, delta_time_s);
      process_input( window, myView, delta_time_s );
      

      
      // glMatrixMode(GL_PROJECTION);
      // glLoadIdentity();

      glUseProgram(shaProg->getProgramLoc());
      //TODO:P Vi måste binda shaProg ID till glUseProgram innan vi skickar upp viewPos...
      myView.update();
      auto view_pos = myView.get_view().get_position();
     
      
    
      
      scenMan.draw(myView);
      glUseProgram(lightProg->getProgramLoc());
      

      // glUseProgram(shaProg->getProgramLoc());
      // a_Mesh.render();
      auto g_buffer_data = myView.get_g_buffer();
      
      // glUseProgram( lightProg->getProgramLoc() );
      glClearColor( 0.4f, 0.6, 1.0, 1.0f );

      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      glActiveTexture( GL_TEXTURE0 );
      glBindTexture( GL_TEXTURE_2D, g_buffer_data.pos_tex_loc );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, g_buffer_data.nor_tex_loc );
      glActiveTexture( GL_TEXTURE2 );
      glBindTexture( GL_TEXTURE_2D, g_buffer_data.spe_tex_loc );
      glActiveTexture( GL_TEXTURE3 );
      glBindTexture( GL_TEXTURE_2D, g_buffer_data.alb_tex_loc ); //TODO:P Denna är den enda som gör något...

      
      for (int i = 0; i < 2; i++) { //TODO:P Endast en ljuskälla ger samma resultat som att använda alla...
         int lightType = lights[i].type;
         Vec3 &dir = lights[i].direction;
         Vec3 &pos = lights[i].position;
         Vec3 &col = lights[i].color;
         Float32 intensity = lights[i].intensity;
         Float32 radius = lights[i].radius;
         Float32 degree = lights[i].degree;
         Float32 spec = lights[i].specularity;


         glUniform1i(glGetUniformLocation(lightProg->getProgramLoc(), ("lights[" + std::to_string(i) + "].type").c_str()), lightType);
         glUniform1fv(glGetUniformLocation(lightProg->getProgramLoc(), ("lights[" + std::to_string(i) + "].dir").c_str()), 1, glm::value_ptr(dir));
         glUniform1fv(glGetUniformLocation(lightProg->getProgramLoc(), ("lights[" + std::to_string(i) + "].pos").c_str()), 1, glm::value_ptr(pos));
         glUniform1fv(glGetUniformLocation(lightProg->getProgramLoc(), ("lights[" + std::to_string(i) + "].rgb").c_str()), 1, glm::value_ptr(col));
         glUniform1f(glGetUniformLocation(lightProg->getProgramLoc(), ("lights[" + std::to_string(i) + "].intensity").c_str()), intensity);
         glUniform1f(glGetUniformLocation(lightProg->getProgramLoc(), ("lights[" + std::to_string(i) + "].radius").c_str()), radius);
         glUniform1f(glGetUniformLocation(lightProg->getProgramLoc(), ("lights[" + std::to_string(i) + "].degree").c_str()), degree);
         glUniform1f(glGetUniformLocation(lightProg->getProgramLoc(), ("lights[" + std::to_string(i) + "].specularity").c_str()), spec);
      }

      // also send light relevant uniforms
      
      // SendAllLightUniformsToShader(shaderLightingPass);
      // shaderLightingPass.setVec3("viewPos", camera.Position);

      glUniform3fv(glGetUniformLocation(lightProg->getProgramLoc(), "view_pos"), 1, glm::value_ptr(view_pos));

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

         glGenBuffers( 1, &quad_vbo );

         glBindVertexArray( quad_vao );

         glBindBuffer( GL_ARRAY_BUFFER, quad_vbo );

         glBufferData( GL_ARRAY_BUFFER,
                       sizeof(quad_verts),
                       &quad_verts,
                       GL_STATIC_DRAW );

         glEnableVertexAttribArray(0);

         glVertexAttribPointer( 0,
                                3,
                                GL_FLOAT,
                                GL_FALSE,
                                5 * sizeof(Float32),
                                (void*)0 );

         glEnableVertexAttribArray(1);

         glVertexAttribPointer( 1,
                                2,
                                GL_FLOAT,
                                GL_FALSE,
                                5 * sizeof(Float32),
                                (void*)(3 * sizeof(Float32)) );
      }
      glBindVertexArray( quad_vao );

      glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

      glBindVertexArray(0);
      
      glClearColor(0.4f, 0.6, 1.0, 1.0f);

      glBindFramebuffer( GL_READ_FRAMEBUFFER,
                         g_buffer_data.buffer_loc );

      glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

      glBlitFramebuffer( 0,
                         0,
                         myView.width,
                         myView.height,
                         0,
                         0,
                         myView.width,
                         myView.height,
                         GL_DEPTH_BUFFER_BIT,
                         GL_NEAREST );

      //glUseProgram( quadProg->getProgramLoc() );



      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
      // float dt_time_s = ImGui::GetIO().DeltaTime; // UNUSED
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

   Float32 camspeed = 50 * delta;
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

bool  firstMouse   = true;
static float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
static float pitch =  0.0f;
static float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

void process_mouse( GLFWwindow *window, Viewport &cam, Float32 delta_time_s  ) {
   static glm::vec3 front;

   if (!mouse_look) 
      return;
   
      
   Float64 xPos, yPos;
   glfwGetCursorPos( window, &xPos, &yPos );

   //COPY PASTE leanopengl
   if ( firstMouse ) {
      lastX = xPos;
      lastY = yPos;
      firstMouse = false;
   }

   bool changed = lastX != xPos;

   Float64 xoffset = xPos - lastX;
   Float64 yoffset = lastY - yPos;
   lastX = xPos;
   lastY = yPos;   

   Float64 sensitivity = 0.05;
   xoffset *= sensitivity;
   yoffset *= sensitivity;

   yaw   += xoffset;
   pitch += yoffset;

   if (pitch > 89.0f)
      pitch = 89.0f;
   if (pitch < -89.0f)
      pitch = -89.0f;

   //cam.transform(Transform::make_rotation());

   if (changed) {
      front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      front.y = sin(glm::radians(pitch));
      front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      glm::normalize(front);

      // Transform rotation;
      // rotation.look_at(front);
      // cam.transform( rotation );

      auto view = cam.get_view(); // get pos, rot, scale
      view.look_at(front);        // set rot
      cam.front = front;
      cam.set_view(view);         // set view to cam view
   }
   //cameraFront = glm::normalize(front);
}

/*
static bool mouse_look = false;

void process_input( GLFWwindow *window, Viewport &cam, Float32 time_delta_s ) {
   static move_speed = 50.0f;

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
   
   Float32 move_distance = move_speed * time_delta_s;

   if ( glfwGetKey(window, GLFW_KEY_W)     == GLFW_PRESS ) {  // forward
      cam.transform( move_distance * forward );
   }
   if ( glfwGetKey(window, GLFW_KEY_S)     == GLFW_PRESS ) { // backward
      auto backward = forward * Transform::make_rotation( Vec3(   0,  179,  0 ) );
      cam.transform( move_distance * backward );
   }
   if ( glfwGetKey(window, GLFW_KEY_A)     == GLFW_PRESS ) { // left
      auto left     = forward * Transform::make_rotation( Vec3(   0,  -89,  0 ) );
      cam.transform( move_distance * left );
   }
   if ( glfwGetKey(window, GLFW_KEY_D)     == GLFW_PRESS ) { // right
      auto right    = forward * Transform::make_rotation( Vec3(   0,   89,  0 ) );
      cam.transform( move_distance * right );
   }
   if ( glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ) { // up
      auto up       = forward * Transform::make_rotation( Vec3( -89,    0,  0 ) );
      cam.transform( move_distance * up );
   }
   if ( glfwGetKey(window, GLFW_KEY_CTRL)  == GLFW_PRESS ) { // down
      auto down     = forward * Transform::make_rotation( Vec3( -89,    0,  0 ) );
      cam.transform( move_distance * down );
   }
}
*/

