#define  STB_IMAGE_IMPLEMENTATION //Den bor här annars kompilerar inte stb_image.h


#include "misc/defs.h"
#include "Config.h"
#include "debug.h"

#include "misc/ImGui/imgui.h"
#include "misc/ImGui/imgui_impl_glfw.h"
#include "misc/ImGui/imgui_impl_opengl3.h"

#include "AssetManager.h"
#include "ShaderManager.h"
#include "Viewport.h"
#include "SceneManager.h"

#include "Light.h"
#include "shadowcasterDebug.h"

#ifdef _DEBUG
   #include "LightDebugger.h"
#endif

#include "ParticleSystem.h" /* @TAG{PS} */
#include <random>           /* used by particle system algorithm defined in main; TODO: move to defs */

// for OpenGL debugging
void APIENTRY glDebugOutput( GLenum         source,
	                          GLenum         type,
	                          GLuint         id,
	                          GLenum         severity,
	                          GLsizei        length,
	                          const GLchar  *message,
	                          const void    *userParam ) {
	// ignore non-significant error/warning codes
	if ( id == 131169 || id == 131185 || id == 131218 || id == 131204 )
      return;

   if ( severity == GL_DEBUG_SEVERITY_NOTIFICATION )
      return;

	std::cout << "_DEBUG MESSAGE (" << id << "): " << message << "\n";

	switch ( source ) {
   	case GL_DEBUG_SOURCE_API:               std::cout << "SOURCE: API";                break;
   	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     std::cout << "SOURCE: Window System";      break;
   	case GL_DEBUG_SOURCE_SHADER_COMPILER:   std::cout << "SOURCE: Shader Compiler";    break;
   	case GL_DEBUG_SOURCE_THIRD_PARTY:       std::cout << "SOURCE: Third Party";        break;
   	case GL_DEBUG_SOURCE_APPLICATION:       std::cout << "SOURCE: Application";        break;
   	case GL_DEBUG_SOURCE_OTHER:             std::cout << "SOURCE: Other";              break;
	}
   std::cout << "\n";

	switch ( type ) {
   	case GL_DEBUG_TYPE_ERROR:               std::cout << "TYPE: Error";                break;
   	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "TYPE: Deprecated Behaviour"; break;
   	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "TYPE: Undefined Behaviour";  break;
   	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "TYPE: Portability";          break;
   	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "TYPE: Performance";          break;
   	case GL_DEBUG_TYPE_MARKER:              std::cout << "TYPE: Marker";               break;
   	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "TYPE: Push Group";           break;
   	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "TYPE: Pop Group";            break;
   	case GL_DEBUG_TYPE_OTHER:               std::cout << "TYPE: Other";                break;
	}
   std::cout << "\n";

	switch ( severity ) {
   	case GL_DEBUG_SEVERITY_HIGH:            std::cout << "SEVERITY: high";             break;
   	case GL_DEBUG_SEVERITY_MEDIUM:          std::cout << "SEVERITY: medium";           break;
   	case GL_DEBUG_SEVERITY_LOW:             std::cout << "SEVERITY: low";              break;
   	case GL_DEBUG_SEVERITY_NOTIFICATION:    std::cout << "SEVERITY: notification";     break;
	}
   std::cout << "\n\n";
}

// function for mouse input (clicking, looking, and picking)
void process_mouse( GLFWwindow   *window, // GLFW window is needed for input
                    Viewport     &cam,    // cam is needed for positional info
                    SceneManager &scene,  // scene is needed for mouse picking
                    Float32       delta_time_s )
{
   // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a
   // direction vector pointing to the right so we initially rotate a bit to the left.
   static Float64 yaw            = -90.0f;
   static Float64 pitch          =   0.0f;
   static Float64 last_x         = {0}; // resolution/2;
   static Float64 last_y         = {0}; // resolution/2;
   static Bool    is_initialized = false;

   Float64 x_pos, y_pos;
   // get the current cursor XY-position from the GLFW window
   glfwGetCursorPos( window, &x_pos, &y_pos );

   // this is so that in case it's the first frame, the current position
   // is used as if it were the last position.
   if ( !is_initialized ) {
      last_x         = cam.forward.x;
      last_y         = cam.forward.y;
      is_initialized = true;
   }


   static SharedPtr<ModelInstance> model;
   static bool open = false; 
   // mouse picking
   if ( !config.is_imgui_toggled  && !open) {                                           // ignore mouse picking if GUI is active
      if ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_1 ) ) {               // if the left mouse button is clicked
         Uint32     obj_id  = scene.get_object_id_at_pixel(x_pos, y_pos, cam); // get the ID of the fragment at the
         auto instance_ptr  = scene.get_instance_ptr(obj_id);                  // current mouse position XY-coordinate.
         if ( instance_ptr != nullptr ) {                                      // if the return value is a nullptr,
            //SharedPtr<ModelInstance> model = instance_ptr;                     // then no valid model instance occupied
             model = instance_ptr;                                               // then no valid model instance occupied
             open = true;
            /////////////////

            

            ////////////////////

            //model->transform( Transform::make_rotation(Vec3(0.0, 0.2, 0.0)) ); // that fragment (e.g. just background).
         }                                                                     // We then rotate the potential match as
         if constexpr ( Config::is_debugging ) // debug terminal output:       // a simple way to get feedback.
            std::cout << "[MOUSE_PICKING]" << x_pos  << ":" << y_pos
                      << ". Model id: "    << obj_id << "\n";
      }
   }

   //if we picked an ModelInstance, open window for manipulating it
   if (model != NULL && open) {

     
      ImGui::Begin("Instance", &open);
      
      //ImGui::Button("Close");
     
      //ImGui::Begin("Instances:"); // begin our Inspection window:
              // draw our window GUI components and do I/O:



      auto transform = model->model_transform;

      Vec3 position = transform.get_position();
      Vec3 scale = transform.get_scale();

      Float32 position_array[3]{ position.x,
                                  position.y,
                                  position.z }; // temp

      Float32 scale_array[3]{ scale.x,
                               scale.y,
                               scale.z }; // temp

      Vec3 rotation(0.0f);

      String id = model->get_model()->get_name();

      ImGui::PushID(id.c_str());
      ImGui::NewLine();
      ImGui::Text("%s:", id.c_str());
      ImGui::InputFloat3("Position", position_array, "%.1f");

      ImGui::SliderAngle("X rotation", &rotation.x);
      ImGui::SliderAngle("Y rotation", &rotation.y);
      ImGui::SliderAngle("Z rotation", &rotation.z);

      ImGui::InputFloat3("Scale", scale_array, "%.2f");
      ImGui::NewLine();

      ImGui::Separator();
      ImGui::PopID();

      Transform trans = Transform(Vec3(0.0f, 0.0f, 0.0f));

      transform = model->model_transform;

      transform.set_position(Vec3(0.0f, 0.0f, 0.0f));

      model->set_transform(transform);
      transform = model->model_transform;

      transform.set_rotation(Vec3(1.0f, 0.0f, 0.0f), rotation.x);
      transform.set_rotation(Vec3(0.0f, 1.0f, 0.0f), rotation.y);
      transform.set_rotation(Vec3(0.0f, 0.0f, 1.0f), rotation.z);


      Transform new_transform(Vec3(position_array[0],
         position_array[1],
         position_array[2]),
         /* temp */ transform.get_rotation(),
         Vec3(scale_array[0],
            scale_array[1],
            scale_array[2]));

      model->set_transform(new_transform);

      ImGui::End();
      
   }
   


   // to avoid repeated recomputations in vain
   Bool has_changed =  last_x != x_pos  ||  last_y != y_pos;
   Float64 x_offset    = x_pos  - last_x;
   Float64 y_offset    = last_y - y_pos;

   last_x    = x_pos; // needs to be done after has_changed
   last_y    = y_pos; // needs to be done after has_changed

   // this is for middle mouse button viewing
   if ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_PRESS ) {
      if ( !config.is_mouse_look_enabled ) {
         glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
         return;
      }
   }
   else glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

   static constexpr Float64 sensitivity = 0.05;
   x_offset *= sensitivity;
   y_offset *= sensitivity;

   // euler angles for mouse rotation
   yaw   += x_offset;
   pitch += y_offset;

   // to avoid rotating beyond 90 degree pitch
   if ( pitch > +89.0f ) pitch = +89.0f;
   if ( pitch < -89.0f ) pitch = -89.0f;

   if ( has_changed ) {
      // cam forward is the unit vector of the camera's forward direction
      cam.forward.x = cos( glm::radians(yaw)) * cos(glm::radians(pitch) );
      cam.forward.y = sin( glm::radians(pitch)                          );
      cam.forward.z = sin( glm::radians(yaw)) * cos(glm::radians(pitch) );
      cam.forward   = glm::normalize( cam.forward );   // normalize

      auto view     = cam.get_view();                  // get view (pos, rot, scale)
      view.look_at( cam.forward, view.get_position()); // rotate view
      cam.set_view( view );                            // update cam view
   }
}

// uses GLFW's callback function to be called just once per key press
void toggle_input_callback( GLFWwindow  *window,
                            Int32        key,
                            Int32        scancode,
                            Int32        action,
                            Int32        mods )
{
   if ( key == GLFW_KEY_ESCAPE  &&  action == GLFW_PRESS )
      glfwSetWindowShouldClose( window, true );
   // TODO: make mouse scroll affect camera fly speed
   // TODO: add keybinding map to Config
   // TODO: refactor I/O into its own module
   // TODO: add imgui keybindings serialization & deserialization
   // TODO: add system to make clicked instance into the active instance
   //       and then add keybindings to affect its position, rotation, scale, etc
   //       (reference: Blender hotkeys)
   if ( key ==  GLFW_KEY_F1  &&  action == GLFW_PRESS ) {
      config.is_mouse_look_enabled = !config.is_mouse_look_enabled;
      // disable mouse cursor if mouse look gets enabled (and vice versa)
      if ( config.is_mouse_look_enabled )
         glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
      else
         glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
   }
   if ( key ==  GLFW_KEY_F2  &&  action == GLFW_PRESS )
      config.is_wireframe_mode = !config.is_wireframe_mode; // used in SceneManager::Draw()
   if ( key ==  GLFW_KEY_F3  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::composite;
   if ( key ==  GLFW_KEY_F4  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::albedo;
   if ( key ==  GLFW_KEY_F5  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::normals;
   if ( key ==  GLFW_KEY_F6  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::specular;
   if ( key ==  GLFW_KEY_F7  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::positional;
   if ( key ==  GLFW_KEY_F8  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::emissive;
   if ( key ==  GLFW_KEY_F9  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::textureless;
   if ( key == GLFW_KEY_F10  &&  action == GLFW_PRESS)
	   config.render_mode = RenderMode::picking;
   if ( key == GLFW_KEY_F11  &&  action == GLFW_PRESS)
      config.render_mode = RenderMode::displacement;
   if ( key == GLFW_KEY_F12  &&  action == GLFW_PRESS )
      config.is_imgui_toggled = !config.is_imgui_toggled;
}


//TODO: If holding Space then while holding it we also  start holding CTRL, we will Change direction
//      If holding CTRL then while holding it we also  start holding  Space, we will NOT Change direction....
// gets called each frame, GLFW is used to read input
void process_input( GLFWwindow  *window,
                    Viewport    &cam,
                    Float32      time_delta_ms )
{
   Float32    move_distance = config.fly_move_speed * (time_delta_ms * 1000);
   Transform  offset;

   // logic for moving the camera based off of user WASD input
   // and the viewport's current position and direction.
   if ( glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( 1.0f, 1.0f, 1.0f )
                  *
                  cam.forward * move_distance
               );
      cam.transform( offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( -1.0f, -1.0f, -1.0f )
                  *
                  cam.forward * move_distance
               );
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( 1.0f, 1.0f, 1.0f )
                  *
                  glm::normalize( glm::cross( -cam.forward, Vec3(0.0, 1.0f, 0.0f) ) ) * move_distance
               );
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( -1.0f, -1.0f, -1.0f )
                  *
                  glm::normalize( glm::cross( -cam.forward, Vec3(0.0f, 1.0f, 0.0f) ) ) * move_distance
               );
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( 0.0f, -1.0f, 0.0f) * move_distance
               );
      cam.transform(offset);

   }
   if ( glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( 0.0f, 1.0f, 0.0f ) * move_distance
               );
      cam.transform(offset);
   }
}


// (currently deprecated) TODO: refactor into debug.h/cpp
[[deprecated]]
void draw_camera_debug_window( Vec3    &position,
                               Vec3    &rotation,
                               Float32 &fov_rad )
{
   if ( !config.is_imgui_toggled )
      return; // early exit if IMGUI is disabled
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
	if ( !glfwInit() ) {
		fprintf( stderr, "[ERROR] Failed to initialize GLFW.\n" );
		return -1;
	}
	// MSAA
	glfwWindowHint( GLFW_SAMPLES, GLFW_DONT_CARE );
	// GLSL v440
	char const *glsl_version = "#version 440";
	// OpenGL v4.4
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 4 );
	// for MacOS; should not be needed
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	// use OpenGL core profile
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	// open a window and create its OpenGL context
	GLFWwindow *window  = glfwCreateWindow( Config::start_width,
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

	// setup input
   glfwSetInputMode(   window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
   glfwSetKeyCallback( window, toggle_input_callback );

   // for enabling OpenGL debugging
   glEnable( GL_DEBUG_OUTPUT );
   glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
   glDebugMessageCallback( glDebugOutput, nullptr );
   glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );

	// ImGui context setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io; // last part just disables -Wunused-variable warning

	// setup ImGui style
	ImGui::StyleColorsDark();

	// setup platform and renderer bindings
	ImGui_ImplGlfw_InitForOpenGL( window, true );
	ImGui_ImplOpenGL3_Init( glsl_version );

	glEnable( GL_DEPTH_TEST );
   //glEnable( GL_NORMALIZE  );
	//glEnable( GL_STENCIL_TEST ); // TODO: implement support for stencil test for g-buffer

   ShaderManager shader_manager {};
   AssetManager  asset_manager  {};

   // TODO: consistent names
   auto lighting_vert_shader            { shader_manager.load_shader( "lightSha.vert" )         };
   auto lighting_frag_shader            { shader_manager.load_shader( "lightSha.frag" )         };
   auto geometry_vert_shader            { shader_manager.load_shader( "g_buffer.vert" )         };
   auto geometry_frag_shader            { shader_manager.load_shader( "g_buffer.frag" )         };
   auto geometry_geom_shader            { shader_manager.load_shader( "g_buffer.geom" )         };
   auto geo_tess_geom_shader            { shader_manager.load_shader( "g_buffer_tess.geom" )    };
   auto geo_tess_vert_shader            { shader_manager.load_shader( "g_buffer_tess.vert")     };
   auto geo_tess_tesc_shader            { shader_manager.load_shader( "g_buffer.tesc")          };
   auto geo_tess_tese_shader            { shader_manager.load_shader( "g_buffer.tese")          };
   auto shadowdepth_vert_shader         { shader_manager.load_shader( "shadow_depth.vert" )     };
   auto shadowdepth_frag_shader         { shader_manager.load_shader( "shadow_depth.frag" )     };
   /*  PS */ auto ps_vert_shader        { shader_manager.load_shader( "particle_system.vert" )  };
   /*  PS */ auto ps_frag_shader        { shader_manager.load_shader( "particle_system.frag" )  };
   //*SSAO*/ auto ssao_vert_shader      { shader_manager.load_shader( "ssao.vert" )             };
   //*SSAO*/ auto ssao_main_frag_shader { shader_manager.load_shader( "ssao.frag" )             };
   //*SSAO*/ auto ssao_blur_frag_shader { shader_manager.load_shader( "ssao_blur.frag" )        };
#ifdef _DEBUG
   auto dbg_line_vert_shader            { shader_manager.load_shader( "line_shader.vert" )      };
   auto dbg_line_frag_shader            { shader_manager.load_shader( "line_shader.frag" )      };
#endif /*DEBUG*/



   //* SSAO */ auto ssao_main_program { shader_manager.create_program({ ssao_vert_shader, ssao_main_frag_shader }) };
   //* SSAO */ auto ssao_blur_program { shader_manager.create_program({ ssao_vert_shader, ssao_blur_frag_shader }) };

   auto geometry_program { shader_manager.create_program( { geometry_frag_shader,
                                                            geometry_geom_shader,
                                                            geometry_vert_shader } ) };

   auto geometry_tessellation_program{ shader_manager.create_program({  geometry_frag_shader,
                                                                        geo_tess_geom_shader,
                                                                        geo_tess_vert_shader,
                                                                        geo_tess_tesc_shader,
                                                                        geo_tess_tese_shader}) };

   auto lighting_program    { shader_manager.create_program({ lighting_frag_shader, lighting_vert_shader }) };

   auto shadowdepth_program { shader_manager.create_program({ shadowdepth_frag_shader, shadowdepth_vert_shader }) };

   auto particle_program    { shader_manager.create_program({ ps_vert_shader, ps_frag_shader }) }; /* @TAG{PS} */

#ifdef _DEBUG
   auto dbg_line_program    { shader_manager.create_program({ dbg_line_vert_shader, dbg_line_frag_shader }) };
#endif /*DEBUG*/

   // add Lightning shader program to SceneManager
   SceneManager  scene_manager { geometry_program
                               , geometry_tessellation_program
                               , lighting_program
                               , shadowdepth_program
                               , particle_program /* @TAG{PS} */
                           #ifdef _DEBUG
                               , dbg_line_program
                           #endif /*DEBUG*/
                    //* SSAO */, ssao_main_program
                    //* SSAO */, ssao_blur_program
                               };

   // creating the camera viewport
   Vec3 view_position { 0.0f, 20.0f, 15.0f };
   auto view = scene_manager.instantiate_viewport( view_position, window, geometry_program );
   view->bind_shader_program( geometry_program ); // TODO: shouldn't be necessary here

   Vector<SharedPtr<Light>> light_instances; // keeps light instances alive & enables access

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                                             Vec3(  0.0f,   0.0f,   0.0f ),
                                                                             Vec3( 14.0f,  9.0f,  120.0f ),
                                                                             Vec3(  1.0f,   0.0f,   0.0f ),
                                                                              0.1,
                                                                             75.0,
                                                                              0.0,
                                                                              1.2 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                                             Vec3( 0.0f,  0.0f,  0.0f ),
                                                                             Vec3( 30.0f, 6.0f,  75.0f ),
                                                                             Vec3( 1.0f,  1.0f,  0.0f ),
                                                                             0.1,
                                                                            99.0,
                                                                             0.0,
                                                                             1.7 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                                             Vec3( 0.0f,  0.0f,  0.0f ),
                                                                             Vec3( 26.0f,  41.0f,  45.0f ),
                                                                             Vec3( 1.0f,  0.0f,  1.0f ),
                                                                              0.1,
                                                                             57.0,
                                                                              0.0,
                                                                              1.2} ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                                             Vec3( 0.0f,  0.0f,  0.0f ),
                                                                             Vec3( 1.0f,  9.0f,  24.0f ),
                                                                             Vec3( 0.0f,  1.0f,  0.0f ),
                                                                              0.1,
                                                                             79.0,
                                                                              0.0,
                                                                              1.5 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                                             Vec3( 0.0f,  0.0f,  1.0f ),
                                                                             Vec3( 11.0f,  14.0f,  191.0f ),
                                                                             Vec3( 0.0f,  1.0f,  1.0f ),
                                                                             0.1,
                                                                           150.0,
                                                                             0.0,
                                                                             0.9 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                                             Vec3( 0.0f,  0.0f,   0.0f ),
                                                                             Vec3( 100.0f,  2.0f,  10.0f ),
                                                                             Vec3( 0.0f,  0.0f,   1.0f ),
                                                                             0.1,
                                                                            81.0,
                                                                             0.0,
                                                                             1.0 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                                             Vec3(  0.0f,  0.0f,  0.0f ),
                                                                             Vec3( 10.0f,  1.0f,  51.0f ),
                                                                             Vec3(  1.0f,  1.0f,  1.0f ),
                                                                             0.1,
                                                                            97.0,
                                                                             0.0,
                                                                             1.9 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                                             Vec3(  0.0f,  0.0f,   0.0f ),
                                                                             Vec3( 150.0f,  25.0f,  200.0f ),
                                                                             Vec3(  1.0f,  0.3f,   0.5f ),
                                                                              0.1,
                                                                            400.0,
                                                                              0.0,
                                                                              1.3 } ) );


   SharedPtr<Model> ape_model = asset_manager.load_model( "ape.obj" );

   // parameters for the Sun / shadowcasting light
   Vec3 poss = Vec3( 101.0f,  100.0f,  100.0f );
   Vec3 dirr = Vec3( -45.0f,    0.0f,  -45.0f );

   Float32 intensity   =   0.5f; // 50%
   Float32 radius      = 570.0f;
   Float32 degree      =   0.0f;
   Float32 specularity =   1.0f;

   auto sun = scene_manager.instantiate_light( Light::Data{ Light::Type::directional,
                                                            dirr,//glm::normalize(poss - dirr),
                                                            poss,
                                                            Vec3( 1.0f,  1.0f,  1.0f ),
                                                            intensity, // percentage
                                                            radius,
                                                            degree,
                                                            specularity } );
   light_instances.push_back( sun );

   auto light_sc = std::make_shared<Shadowcaster>(sun);

   // must initialize before first use set_light_matrix(...) atleast once!
   light_sc->set_Light_matrix(0.1f, glm::length(sun->get_position() - sun->get_direction()), 50, -50, 50, -50, sun->get_position(), sun->get_direction(), Vec3(0.0f, 1.0f, 0.0f));
   scene_manager.set_shadowcasting( light_sc );

   Vector<SharedPtr<ModelInstance>> model_instances; // keeps model instances alive  & enables access

   // we create 64 monkeys (tessellated one outside the loop) in a grid
   model_instances.reserve( 64 );
   for ( auto i=0;  i<63;  ++i ) {
      Float32 n = 9; // spacing
      model_instances.push_back(
         scene_manager.instantiate_model( ape_model,
                                          geometry_program,
                                          Transform( Vec3( n*(i/8)-40,  0.0f,  n*(i%8)-40 ),
                                                     Vec3(       0.0f,  0.0f,        0.0f ),
                                                     Vec3(       1.3f,  1.3f,        1.3f ) ) ) );
   }
   // tessellated ape
   model_instances.push_back(
	   scene_manager.instantiate_model( ape_model,
		                                 geometry_tessellation_program,
		                                 Transform( Vec3( 9*(63 / 8) - 40, 0.0f, 9*(63 % 8) - 40),
			                                         Vec3( 0.0f, 0.0f, 0.0f),
			                                         Vec3( 1.3f, 1.3f, 1.3f ) ), true ) );

   SharedPtr<Model> floor = asset_manager.load_model("floor_64x64(1).obj");

   // tessellated floor
   model_instances.push_back(
      scene_manager.instantiate_model( floor,
                                       geometry_tessellation_program,
                                       Transform( Vec3(  0.0f, 0.0f,  0.0f ),
                                                  Vec3(  0.0f, 0.0f,  0.0f ),
                                                  Vec3( 10.0f, 1.0f, 10.0f ) ), true) );


   // debug tool to see more clearly how Light frustrum looks like
   auto sun_pos = sun->get_position();
   auto sun_dir = sun->get_direction();
   ShadowcasterDebug sundbg = ShadowcasterDebug(  light_sc,
                                                 &asset_manager,
                                                 &scene_manager,
                                                 &model_instances,
                                                  geometry_program,
                                                 &sun_pos,
                                                 &sun_dir );


   // set tessellated related values (that will later be uploaded as uniforms)
   Float32 displacement_factor =  1.5f;
   Float32 tess_percent        = -0.9f;

   glUseProgram( lighting_program->get_location() );

   // @TAG{TEXTURE_CHANNEL}
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_albedo" ), 0 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_spec"   ), 1 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_norm"   ), 2 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_emit"   ), 3 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_disp"   ), 4 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_pos"    ), 5 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_pic"    ), 6 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "shadowMap"    ), 7 );

   // glEnable( GL_CULL_FACE ); // TODO: enable
   glDisable(  GL_CULL_FACE );  // TODO: disable

   // TODO: fix transparency handling (by changing RGBA alpha usage)
   // glEnable( GL_BLEND );
   // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

/* @TAG{PS} */
/* PS */ // Lambda function encapsulating particle logic
/* PS */ auto ps_logic = [] ( ParticleSystem::Data &data, Float32 delta_t_ms ) {
/* PS */    using  Particle = ParticleSystem::Data::Particle;
/* PS */
/* PS */    static Float32 const births_per_s      { 120.0f                 }; // if too high and Config::particle_max_count is too low,
/* PS */    static Float32 const ms_between_births { 1'000.f / births_per_s }; // ^ might cause the particles to be created in clusters.
/* PS */    static Float32 const avg_lifespan_ms   { 9'000.0f               };
/* PS */    static Float32 const avg_mass_kg       {     0.01f              };
/* PS */    static Float32 const avg_scale         {     0.50f              };
/* PS */    static Uvec4   const colour_rgba       { 255, 255, 255, 255     };
/* PS */    static Float32 const radius_m          { 120.f                  };
/* PS */    static Float32       time_pool_ms      { .0f                    };
/* PS */    static Float32       elapsed_time      {  0                     };
/* PS */    elapsed_time += delta_t_ms;
/* PS */    time_pool_ms += (delta_t_ms);
/* PS */
/* PS */    static Float32 sin[8] {};
/* PS */    static Float32 cos[8] {};
/* PS */    for ( int i=0;  i<8;  ++i ) {
/* PS */       sin[i] = (0.10 * glm::sin(glm::radians((elapsed_time * 0.01 + i*53))));
/* PS */       cos[i] = (0.05 * glm::cos(glm::radians((elapsed_time * 0.01 + i*37))));
/* PS */    }
/* PS */
/* PS */    for ( auto i = 0;  i < data.count;  ++i ) {
/* PS */        auto &particle          =  data.data[i]; // TODO: rename in ParticleSystem
/* PS */        // position:
/* PS */        particle.spatial[1]    +=  (-.005f -(i % 20)/43) * delta_t_ms;
/* PS */        particle.spatial[0]    += sin[(i+31)%8];
/* PS */        particle.spatial[2]    += cos[(i+47)%8];
/* PS */        // scale:
/* PS */        particle.spatial[3]     = avg_scale * (1-(avg_lifespan_ms-particle.time_ms_left)/avg_lifespan_ms);
/* PS */        particle.time_ms_left  -= delta_t_ms;
/* PS */
/* PS */    }
/* PS */
/* PS */    std::random_device rd;
/* PS */    std::mt19937 mt( rd() );
/* PS */    std::uniform_real_distribution<Float32> dist( -radius_m, +radius_m );
/* PS */    std::uniform_real_distribution<Float32> scale_dist( -.30f, +.30f );
/* PS */    while ( time_pool_ms > ms_between_births ) {
/* PS */       data.add( Particle { colour_rgba,
/* PS */                            Vec4 { dist(mt), 40.0f, dist(mt), avg_scale }, // random position
/* PS */                            Vec3 { .0f, -.01f, .0f },
/* PS */                            avg_lifespan_ms,
/* PS */                            avg_mass_kg } );
/* PS */       time_pool_ms -= ms_between_births;
/* PS */    }
/* PS */ };
/* PS */
/* PS */ auto snowflake_dif  = std::make_shared<DiffuseTexture>      ( FilePath{ FileType::texture, "snowflake_dif.png"  });
/* PS */ auto snowflake_nor  = std::make_shared<NormalTexture>       ( FilePath{ FileType::texture, "snowflake_nor.png"  });
/* PS */ auto snowflake_spec = std::make_shared<SpecularTexture>     ( FilePath{ FileType::texture, "snowflake_spec.png" });
/* PS */ auto snowflake_emit = std::make_shared<EmissiveTexture>     ( FilePath{ FileType::texture, "snowflake_emit.png" });
/* PS */ auto snowflake_disp = std::make_shared<DisplacementTexture> ( /* no displacement texture */ );
/* PS */
/* PS */ TextureSet snowflake_tex { snowflake_dif, snowflake_nor, snowflake_spec, snowflake_emit, snowflake_disp };
/* PS */
/* PS */ auto ps { std::make_shared<ParticleSystem>( Transform::make_translation(Vec3{.0f, 3.0f, .0f}), snowflake_tex, ps_logic ) };
/* PS */ // instantiating particle system
/* PS */ scene_manager.instantiate_particle_system( ps ); // TODO: revamp in SceneManager
/* PS */ ps->start(); // starting it

auto debug_gui = DebugGUI { &scene_manager, &asset_manager, /*UGLY:*/&*view }; /*DEBUG*/ /*GUI*/

// main loop:
	while ( !glfwWindowShouldClose(window) ) {
      Float32 delta_time_ms { ImGui::GetIO().DeltaTime / 1000.f };
      scene_manager.update( delta_time_ms * 1'000'000.f );

      if constexpr ( Config::is_debugging ) {
         static Uint32 frame = 0;
         printf( "-------------------------------- Frame %5d --------------------------------\n", frame++ );
      }

      glUseProgram( geometry_tessellation_program->get_location() );
      glUniform1f( glGetUniformLocation(geometry_tessellation_program->get_location(), "displacement_factor"),
                   displacement_factor );

      glUniform1f( glGetUniformLocation(geometry_tessellation_program->get_location(), "tess_percent"),
                   tess_percent );

		// poll & handle events such as window resizing, input, etc
		glfwPollEvents();

		// start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

      //Removed since we can open them by clicking
      //scene_manager.draw_debug_scene_inspection();

      // TODO: refactor into debug.h/cpp
      if ( config.is_imgui_toggled ) {
         ImGui::Begin( "Settings:" );
         ImGui::SliderFloat( "Move speed", &config.fly_move_speed, 0.0f, 85.0f );
         ImGui::End();

         ImGui::Begin("Tessellation Settings:");
         ImGui::SliderFloat("Displacement Factor: ", &displacement_factor, -10.0f, 10.0f);
         ImGui::SliderFloat("Tessellation level Percent: ", &tess_percent, -1.0f, 1.0f);
         ImGui::End();
      }

      debug_gui.draw(); /*DEBUG*/ /*GUI*/

/*light_dbg*/ Array<Float32,4> corners = light_sc->getCorners();
/*light_dbg*/ for ( auto &light : light_instances )
/*light_dbg*/    debug::lightsource( light, scene_manager );
/*lightcast*/ light_sc->set_Light_matrix( 0.1f,
/*lightcast*/                             glm::length(sun->get_position() - sun->get_direction()),
/*lightcast*/                             corners[0],
/*lightcast*/                             corners[1],
/*lightcast*/                             corners[2],
/*lightcast*/                             corners[3],
/*lightcast*/                             sun->get_position(),
/*lightcast*/                             sun->get_direction(),
/*lightcast*/                             Vec3(0.0f, 1.0f, 0.0f) ); // up vector?
/*light_dbg*/ sundbg.light_caster_debugg_tool_render();

      process_mouse( window, *view, scene_manager, delta_time_ms );
      process_input( window, *view, delta_time_ms );


/*dance*/ auto mo  = model_instances[10];
/*dance*/ mo->transform( Transform::make_rotation(Vec3(1.0f, 0.0f, 0.0f), glm::radians(30.0f) ) );
/*dance*/ auto mo2 = model_instances[11];
/*dance*/ auto mdl = mo2->model_transform;
/*dance*/ static Float32 g = 0.0f;
/*dance*/ static Float32 h = 0.0f;
/*dance*/ static bool  hm = true;
/*dance*/ static bool  isPressed = false;
/*dance*/ static float time = ImGui::GetTime();
/*dance*/ float curTime  = ImGui::GetTime();
/*dance*/ float timepast = curTime - time;
/*dance*/ if (glfwGetKey(window, GLFW_KEY_P)&& timepast > 0.25) {
/*dance*/    isPressed = !isPressed;
/*dance*/    time = ImGui::GetTime();
/*dance*/ }
/*dance*/ if (true && isPressed) {
/*dance*/    if (g > 2)
/*dance*/       hm = false;
/*dance*/    else if (g < -2)
/*dance*/       hm = true;
/*dance*/    if (hm)
/*dance*/       g += 0.01f;
/*dance*/    else g -= 0.01f;
/*dance*/    h += 0.01f;
/*dance*/    for (int i = 0; i < 64-1; i++) {
/*dance*/       Float32 size = 0.1f;
/*dance*/       model_instances[i]->set_transform(Transform::make_translation(model_instances[i]->model_transform.get_position()));
/*dance*/       model_instances[i]->transform(Transform::make_rotation(Vec3((Float32)glm::cos(15 * g*(((i % 4))% 2)), (Float32)glm::cos(12*g*((i % 3))), (Float32)glm::sin(g/2* (i % 2)))) * Transform::make_translation(Vec3((Float32)glm::sin(g*i)*((i % 3)* size), (Float32)glm::sin(g* (i % 2)* size)*0.02 , (Float32)glm::sin(g/10)*((i%5) % 2))));
/*dance*/    }
/*dance*/    auto cur = model_instances[63]->model_transform;
/*dance*/    Float32 radius = 30;
/*dance*/    Vec3 rotateAround(0.0,0.0,0.0);//Might be wrong
/*dance*/    Float32 speed = 5;
/*dance*/    model_instances[63]->set_transform(Transform::make_translation((Vec3((radius) * glm::cos(speed * h + rotateAround.x) + rotateAround.y, rotateAround.z, (radius) * glm::sin(speed * h  + rotateAround.x) + rotateAround.y))));
/*dance*/ }

      // draw scene content
      scene_manager.draw( *view );

      // draw imgui components
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

      // finalize frame
      glfwMakeContextCurrent( window );
      glfwSwapBuffers( window );

      // title changes
      auto fps        = ImGui::GetIO().Framerate;
      auto mspfps     = 1000.0f / ImGui::GetIO().Framerate;
      char title[512] = {};
      snprintf( title, 512, "3D Project ─ %3.1f FPS (%3.1fms/frame)", fps, mspfps );
      glfwSetWindowTitle( window, title );


	} // main loop end

   // cleanup:
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow( window );
	glfwTerminate(); // close OpenGL window & terminate GLFW
	return 0; // successful exit
}
