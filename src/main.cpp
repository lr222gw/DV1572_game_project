#define STB_IMAGE_IMPLEMENTATION //Den bor här annars kompilerar inte stb_image.h

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

// temp:
//    #include <cstdio>
//    #include <unistd.h>

#include "shadowcasterDebug.h"

// #include "misc/stb_image.h"
// #include <range/v3/all.hpp>

/* @TAG{PS} */
#include "ParticleSystem.h"
#include <random> /* used by particle system algorithm defined in main */


Float32 g_move_speed            = 25.0f; // TODO: refactor away ugly globalness
Bool    g_is_mouse_look_enabled = false;
// temp debug for mouse picking


// For opengl debuging
void APIENTRY glDebugOutput( GLenum        source,
	                          GLenum        type,
	                          GLuint        id,
	                          GLenum        severity,
	                          GLsizei       length,
	                          const GLchar *message,
	                          const void   *userParam ) {
	// ignore non-significant error/warning codes
	if ( id == 131169 || id == 131185 || id == 131218 || id == 131204 )
      return;

   if ( severity == GL_DEBUG_SEVERITY_NOTIFICATION ) return;

	std::cout << "DEBUG MESSAGE (" << id << "): " << message << "\n";

	switch ( source ) {
   	case GL_DEBUG_SOURCE_API:             std::cout << "SOURCE: API";             break;
   	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "SOURCE: Window System";   break;
   	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "SOURCE: Shader Compiler"; break;
   	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "SOURCE: Third Party";     break;
   	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "SOURCE: Application";     break;
   	case GL_DEBUG_SOURCE_OTHER:           std::cout << "SOURCE: Other";           break;
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
   	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "SEVERITY: high";         break;
   	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "SEVERITY: medium";       break;
   	case GL_DEBUG_SEVERITY_LOW:          std::cout << "SEVERITY: low";          break;
   	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "SEVERITY: notification"; break;
	}
   std::cout << "\n\n";
}

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


void process_mouse( GLFWwindow   *window,
                    Viewport     &cam,
                    SceneManager  scene,
                    Float32       delta_time_s )
{
   // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a
   // direction vector pointing to the right so we initially rotate a bit to the left.
   static Float64 yaw         = -90.0f;
   static Float64 pitch       =  0.0f;
   static Float64 last_x      =  800.0f / 2.0;
   static Float64 last_y      =  600.0 / 2.0;
   static Bool    first_mouse = true;

   Float64 x_pos, y_pos;

   glfwGetCursorPos( window, &x_pos, &y_pos );

   if ( first_mouse ) {
      last_x      = cam.forward.x;
      last_y      = cam.forward.y;
      first_mouse = false;
   }

   // mouse picking
   if ( true ) { // getting crashes again
      if ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_1 ) ) {
         Uint32     obj_id = scene.get_object_id_at_pixel(x_pos, y_pos, cam);
         auto instance_ptr = scene.get_instance_ptr(obj_id);
         if ( instance_ptr != nullptr ) {
            SharedPtr<ModelInstance> model = instance_ptr;
            model->transform( Transform::make_rotation(Vec3(0.0, 1.0, 0.0)) );
         }
         if constexpr ( Config::is_debugging )
            std::cout << "[MOUSE_PICKING]" << x_pos  << ":" << y_pos
                      << ". Model id: "    << obj_id << "\n";
      }
   }


   Bool has_changed = last_x != x_pos || last_y != y_pos;

   Float64 x_offset = x_pos - last_x;
   Float64 y_offset = last_y - y_pos;
   last_x           = x_pos;
   last_y           = y_pos;

   if ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_PRESS ) {
      if ( !g_is_mouse_look_enabled ) {
         glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
         return;
      }
   }
   else glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );


   // 'http://justsomething.co/wp-content/uploads/2013/11/guns-replaced-thumbs-up-20.jpg'

   Float64 sensitivity = 0.05;
   x_offset *= sensitivity;
   y_offset *= sensitivity;

   yaw   += x_offset;
   pitch += y_offset;

   if ( pitch > 89.0f )
      pitch =  89.0f;
   if ( pitch < -89.0f )
      pitch = -89.0f;

   if ( has_changed ) {
      cam.forward.x = cos( glm::radians(yaw)) * cos(glm::radians(pitch) );
      cam.forward.y = sin( glm::radians(pitch)                          );
      cam.forward.z = sin( glm::radians(yaw)) * cos(glm::radians(pitch) );
      cam.forward   = glm::normalize( cam.forward );

      auto view = cam.get_view();  // get view (pos, rot, scale)

      view.look_at( cam.forward, view.get_position()); // rotate view
      cam.set_view( view );        // update cam view
      //Transform().set_rotation()

      //auto hm = Transform(view.get_position(), cam.forward);
      //auto hm = Transform::make_rotation(( cam.forward));

      //cam.transform(hm);

      //cam.transform(Transform::make_rotation(Vec3(1.0f, 0.0f, 0.0f), glm::radians(0.1f)));

      //cam.transform(Transform::make_rotation(Vec3(1.0f, 0.0f, 0.0f), cam.forward.x));
      //auto t = Transform::make_rotation(cam.forward);
      //cam.transform(t);
      //auto rotation = Transform::make_rotation(cam.forward);

      //TODO:

   }
}

//void process_picking(GLFWwindow *window, SceneManager scene, Viewport view)
//{
//
//}

void toggle_input_callback( GLFWwindow  *window,
                            Int32        key,
                            Int32        scancode,
                            Int32        action,
                            Int32        mods )
{
   if ( key == GLFW_KEY_ESCAPE  &&  action == GLFW_PRESS )
      glfwSetWindowShouldClose( window, true );

   if ( (key == GLFW_KEY_F1 )  &&  action == GLFW_PRESS ) {
      g_is_mouse_look_enabled = !g_is_mouse_look_enabled;
      if ( g_is_mouse_look_enabled )
         glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
      else
         glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
   }


   if ( key == GLFW_KEY_F2  &&  action == GLFW_PRESS )
      config.is_wireframe_mode = !config.is_wireframe_mode; // used in SceneManager::Draw()
   if ( key == GLFW_KEY_F3  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::composite;
   if ( key == GLFW_KEY_F4  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::albedo;
   if ( key == GLFW_KEY_F5  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::normals;
   if ( key == GLFW_KEY_F6  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::specular;
   if ( key == GLFW_KEY_F7  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::positional;
   if ( key == GLFW_KEY_F8  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::emissive;
   if ( key == GLFW_KEY_F9  &&  action == GLFW_PRESS )
      config.render_mode = RenderMode::textureless;
   if ( key == GLFW_KEY_F10 &&  action == GLFW_PRESS)
	   config.render_mode = RenderMode::picking;
   if ( key == GLFW_KEY_F12 &&  action == GLFW_PRESS )
      config.is_imgui_toggled = !config.is_imgui_toggled;


}



void process_input( GLFWwindow  *window,
                    Viewport    &cam,
                    Float32      time_delta_ms )
{

   // glfwSetInputMode( window, GLFW_STICKY_KEYS, 1 );

   Float32    move_distance = g_move_speed * (time_delta_ms * 1000);
   Transform  offset;


   if ( glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( 1.0f, 1.0f, move_distance )
                  *
                  cam.forward
               );
      cam.transform( offset);

   }
   if ( glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( -1.0f, -1.0f, -move_distance )
                  *
                  cam.forward
               );
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( move_distance, 1.0f, 1.0f )
                  *
                  glm::cross( -cam.forward, Vec3(0.0, 1.0f, 0.0f) )
               );
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( -move_distance, -1.0f, -1.0f )
                  *
                  glm::cross( -cam.forward, Vec3(0.0f, 1.0f, 0.0f) )
               );
      cam.transform(offset);
   }
   if ( glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( 0.0f, -move_distance, 0.0f)
               );
      cam.transform(offset);

   }
   if ( glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ) {
      offset = Transform::make_translation(
                  Vec3( 0.0f, move_distance, 0.0f )
               );
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



// TODO: refactor into debug.h/cpp
void draw_camera_debug_window( Vec3    &position,
                               Vec3    &rotation,
                               Float32 &fov_rad )
{
   if ( !config.is_imgui_toggled )
      return;

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


// void print_cwd() {
//    char path[1024] = "";
//    getcwd( path, 1024 );
//    printf( "%s\n", path ); //std::filesystem::current_path().c_str() );
// }


Int32 main( Int32 argc, char const *argv[] ) {
	// initialise GLFW
	glewExperimental = true; // <- needed for core profile
	if ( !glfwInit() ) {
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
   glfwSetKeyCallback( window, toggle_input_callback);

   // for enabeling opengl debugging
   glEnable(GL_DEBUG_OUTPUT);
   glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
   glDebugMessageCallback(glDebugOutput, nullptr);
   glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

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

   ShaderManager shader_manager{};
   AssetManager  asset_manager{};

   auto lighting_vert_shader    { shader_manager.load_shader( "lightSha.vert" )        }; // TODO: rename files
   auto lighting_frag_shader    { shader_manager.load_shader( "lightSha.frag" )        }; // TODO: rename files
   auto geometry_vert_shader    { shader_manager.load_shader( "g_buffer.vert" )        }; // TODO: rename files
   auto geometry_frag_shader    { shader_manager.load_shader( "g_buffer.frag" )        }; // TODO: rename files
   auto geometry_geom_shader    { shader_manager.load_shader( "g_buffer.geom" )        };
   //auto geo_tess_vert_shader    { shader_manager.load_shader( "g_buffer_tess.vert") };
   auto geo_tess_tesc_shader    { shader_manager.load_shader( "g_buffer.tesc")         };
   auto geo_tess_tese_shader    { shader_manager.load_shader( "g_buffer.tese")         };
   auto shadowdepth_vert_shader { shader_manager.load_shader( "shadow_depth.vert" )    };
   auto shadowdepth_frag_shader { shader_manager.load_shader( "shadow_depth.frag" )    };
   /* PS */ auto ps_vert_shader { shader_manager.load_shader( "particle_system.vert" ) }; //
   /* PS */ //auto ps_geom_shader { shader_manager.load_shader( "particle_system.geom" ) }; //  /* @TAG{PS} */
   /* PS */ auto ps_frag_shader { shader_manager.load_shader( "particle_system.frag" ) }; //


   auto geometry_program      { shader_manager.create_program( { geometry_frag_shader,
                                                                 geometry_geom_shader,
                                                                 geometry_vert_shader } ) };

   auto geometry_tessellation_program{ shader_manager.create_program({  geometry_frag_shader,
                                                                        geometry_geom_shader,
                                                                        geometry_vert_shader,
                                                                        geo_tess_tesc_shader,
                                                                        geo_tess_tese_shader}) };

   auto lighting_program      { shader_manager.create_program({ lighting_frag_shader, lighting_vert_shader }) };

   auto shadowdepth_program   { shader_manager.create_program({ shadowdepth_frag_shader, shadowdepth_vert_shader }) };

   auto particle_program      { shader_manager.create_program({ ps_vert_shader, ps_frag_shader }) };    /* @TAG{PS} */

   //Add Lightning program to Scenemanager
   SceneManager  scene_manager{ geometry_program, geometry_tessellation_program, lighting_program , shadowdepth_program, particle_program }; /* @TAG{PS} */

   Vector<SharedPtr<Light>> light_instances;

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                               Vec3(  0.0f,   0.0f,   0.0f ),
                                                               Vec3( 10.0f,  10.0f,  10.0f ),
                                                               Vec3(  1.0f,   0.0f,   0.0f ),
                                                                0.1,
                                                               14.0,
                                                                0.0,
                                                                1.0 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                  Vec3( 0.0f,  0.0f,  0.0f ),
                                                  Vec3( 1.0f,  4.0f,  5.0f ),
                                                  Vec3( 1.0f,  1.0f,  0.0f ),
                                                  0.1,
                                                  7.0,
                                                  0.0,
                                                  1.0 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                  Vec3( 0.0f,  0.0f,  0.0f ),
                                                  Vec3( 2.0f,  1.0f,  5.0f ),
                                                  Vec3( 1.0f,  0.0f,  1.0f ),
                                                   0.1,
                                                  17.0,
                                                   0.0,
                                                   1.0} ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                  Vec3( 0.0f,  0.0f,  0.0f ),
                                                  Vec3( 1.0f,  5.0f,  6.0f ),
                                                  Vec3( 0.0f,  1.0f,  0.0f ),
                                                   0.1,
                                                  11.0,
                                                   0.0,
                                                   1.0 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                    Vec3( 0.0f,  0.0f,  1.0f ),
                                    Vec3( 3.0f,  3.0f,  1.0f ),
                                    Vec3( 0.0f,  1.0f,  1.0f ),
                                    0.1,
                                    2.0,
                                    0.0,
                                    1.0 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                    Vec3( 0.0f,  0.0f,   0.0f ),
                                    Vec3( 1.0f,  2.0f,  10.0f ),
                                    Vec3( 0.0f,  0.0f,   1.0f ),
                                    0.1,
                                    1.0,
                                    0.0,
                                    1.0 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                    Vec3(  0.0f,  0.0f,  0.0f ),
                                    Vec3( 10.0f,  0.0f,  5.0f ),
                                    Vec3(  1.0f,  1.0f,  1.0f ),
                                    0.1,
                                    7.0,
                                    0.0,
                                    1.0 } ) );

   light_instances.push_back( scene_manager.instantiate_light( Light::Data { Light::Type::point,
                                                Vec3(  0.0f,  0.0f,   0.0f ),
                                                Vec3( 10.0f,  5.0f,  10.0f ),
                                                Vec3(  1.0f,  0.3f,   0.5f ),
                                                 0.1,
                                                17.0,
                                                 0.0,
                                                 1.0 } ) );

   //SharedPtr<Model> nanosuit_model = asset_manager.load_model( "ape.obj" );

   SharedPtr<Model> ape_model = asset_manager.load_model( "ape.obj" );


   //SharedPtr<Model> isle = asset_manager.load_model("Small Tropical Island.obj");

   Vec3 poss = Vec3( 101.0f,  100.0f,  100.0f );
   Vec3 dirr = Vec3( -45.0f,    0.0f,  -45.0f );

   Float32 intensity   =   0.5f; // 50%
   Float32 radius      = 570.0f;
   Float32 degree      =   0.0f;
   Float32 specularity =   1.0f;

   auto sun = scene_manager.instantiate_light( Light::Data{ Light::Type::directional,
                                                            glm::normalize(poss - dirr),
                                                            poss,
                                                            Vec3( 1.0f,  1.0f,  1.0f ),
                                                            intensity, //Percentage
                                                            radius,
                                                            degree,
                                                            specularity } );
   light_instances.push_back( sun );

   SharedPtr<Shadowcaster> light_sc = std::make_shared<Shadowcaster>(sun);

   //Must initialize before first use (set_light_matrix(...) atleast once!)
   light_sc->set_Light_matrix(0.1f, glm::length(poss - dirr), 50, -50, 50, -50, poss, dirr, Vec3(0.0f, 1.0f, 0.0f));
   scene_manager.set_shadowcasting( light_sc );

   Vector<SharedPtr<ModelInstance>> model_instances;
   //model_instances.push_back(scene_manager.instantiate_model(isle,geometry_program, Transform(Vec3(1*(2 / 8) -40, 150.0f, 2*(2 % 8) - 40),
   //   Vec3(0.0f, 0.0f, 0.0f),
   //   Vec3(.3f, .3f, .3f))));

   model_instances.reserve( 64 );
   for ( auto i=0;  i<64;  ++i ) {
      Float32 n = 9; // spacing
      model_instances.push_back(
         scene_manager.instantiate_model( ape_model,
                                          geometry_program,
                                          Transform( Vec3( n*(i/8)-40,  0.0f,  n*(i%8)-40 ),
                                                     Vec3(       0.0f,  0.0f,        0.0f ),
                                                     Vec3(       1.3f,  1.3f,        1.3f ) ) ) );
   }

   SharedPtr<Model> floor = asset_manager.load_model("floor.obj");
   model_instances.push_back(scene_manager.instantiate_model(floor,
         geometry_program,
         Transform(Vec3(0.0, 0.0, 0.0),
         Vec3(0.0f, 0.0f ,0.0f),
         //Vec3(0.0f, 0.0, 0.0f),
         Vec3(100.0f, 1.0f, 100.0f))));


   //Tool to see more clearly how Light frustrum looks like
   ShadowcasterDebug sundbg = ShadowcasterDebug(light_sc, &asset_manager, &scene_manager, &model_instances, geometry_program, &poss, &dirr);

   //SunApe->set_transform()

   /* TODO */ Vec3       cam_rotations {  0.0f,   0.0f,   0.0f };
   /* TODO */ Vec3       cam_position  {  0.0f, 20.0f,  15.0f };
   /* TODO */ Transform  cam_transform;
   /* TODO */ Float32    fov_rad { Config::fov_rad }; // 90 degrees
   /* TODO */ Viewport view { cam_position, window, geometry_program, fov_rad };
   /* TODO */ view.bind_shader_program( geometry_program );
   /* TODO */ //TODO: remove when we dont want to se dogass
   /* TODO */ view._g_buffer_init();
   /* TODO */ //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   glUseProgram( lighting_program->get_location() );

   // @TAG{TEXTURE_CHANNEL}
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_albedo" ), 0 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_spec"   ), 1 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_norm"   ), 2 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_emit"   ), 3 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_pos"    ), 4 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "g_tex_pic"    ), 5 );
   glUniform1i( glGetUniformLocation( lighting_program->get_location(), "shadowMap"    ), 6 );

   //glEnable(GL_CULL_FACE);
   //glEnable( GL_BLEND );
   //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );





   /* @TAG{PS} */
   /* PS */ auto ps_logic = [] ( ParticleSystem::Data &data, Float32 delta_t_ms ) {
   /* PS */    using  Particle = ParticleSystem::Data::Particle;
   /* PS */
   /* PS */    static Float32 const births_per_s       { 240.0f                 };
   /* PS */    static Float32 const ms_between_births  { 1'000.f / births_per_s };
   /* PS */    static Float32 const avg_lifespan_ms    { 9'000.0f               };
   /* PS */    static Float32 const avg_mass_kg        {     0.01f              };
   /* PS */    static Float32 const avg_scale          {     0.50f              };
   /* PS */    static Uvec4   const colour_rgba        { 255, 255, 255, 255     };
   /* PS */    static Float32 const radius_m           { 120.f                  };
   /* PS */    static Float32       time_pool_ms       { .0f                    };
   /* PS */    static Float32       elapsed_time       {  0                     };
   /* PS */    elapsed_time += delta_t_ms;
   /* PS */    time_pool_ms += (delta_t_ms);
   /* PS */
               static Float32 sin[8] {};
               static Float32 cos[8] {};
               for ( int i=0;  i<8;  ++i ) {
                  sin[i] = (0.10 * glm::sin(glm::radians((elapsed_time * 0.01 + i*53))));
                  cos[i] = (0.05 * glm::cos(glm::radians((elapsed_time * 0.01 + i*37))));
               }

   /* PS */    for ( auto i = 0;  i < data.count;  ++i ) {
   /* PS */        auto &particle          =  data.data[i]; // TODO: rename in ParticleSystem
   /* PS */        //Pos
   /* PS */        particle.spatial[1]    +=  (-.005f -(i % 20)/43) * delta_t_ms;
   /* PS */        particle.spatial[0]    += sin[(i+31)%8];
   /* PS */        particle.spatial[2]    += cos[(i+47)%8];
   /* PS */        //Scale
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
   /* PS */ auto snowflake_dif  = std::make_shared<DiffuseTexture>  ( FilePath{ FileType::texture, "snowflake_dif.png"  });
   /* PS */ auto snowflake_nor  = std::make_shared<NormalTexture>   ( FilePath{ FileType::texture, "snowflake_nor.png"  });
   /* PS */ auto snowflake_spec = std::make_shared<SpecularTexture> ( FilePath{ FileType::texture, "snowflake_spec.png" });
   /* PS */ auto snowflake_emit = std::make_shared<EmissiveTexture> ( FilePath{ FileType::texture, "snowflake_emit.png" });
   /* PS */
   /* PS */ TextureSet snowflake_tex { snowflake_dif, snowflake_nor, snowflake_spec, snowflake_emit };
   /* PS */
   /* PS */ auto ps { std::make_shared<ParticleSystem>( Transform::make_translation(Vec3{.0f, 3.0f, .0f}), snowflake_tex, ps_logic ) };
   /* PS */ scene_manager.instantiate_particle_system( ps ); // TODO: revamp in SceneManager
   /* PS */ ps->start();

// main loop:
	while ( !glfwWindowShouldClose(window) ) {
      Float32 delta_time_ms { ImGui::GetIO().DeltaTime / 1000.f };
      scene_manager.update( delta_time_ms * 1'000'000.f );

      if constexpr ( Config::is_debugging ) {
         static Uint32 frame = 0;
         printf( "-------------------------------- Frame %5d --------------------------------\n", frame++ );
      }

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

      // TODO: bryt ut till debug
      if ( config.is_imgui_toggled ) {
         ImGui::Begin( "Settings:" );
         ImGui::SliderFloat( "Move speed", &g_move_speed, 0.0f, 25.0f );
         ImGui::End();
      }

      Array<Float32,4> corners = light_sc->getCorners();

      for ( auto &light : light_instances )
         debug::lightsource( light, scene_manager );

      light_sc->set_Light_matrix( 0.1f,
                                  glm::length(poss-dirr),
                                  corners[0],
                                  corners[1],
                                  corners[2],
                                  corners[3],
                                  sun->get_position(),
                                  sun->get_direction(),
                                  Vec3(0.0f, 1.0f, 0.0f) ); // up vector?

      sundbg.light_caster_debugg_tool_render();

      process_mouse( window, view, scene_manager, delta_time_ms );
      process_input( window, view, delta_time_ms );

      // glMatrixMode( GL_PROJECTION );
      // glLoadIdentity();

      //auto mo = model_instances[10]->model_transform;
      //auto pos = mo.get_position();
      //mo.set_position(Vec3(0.0f, 0.0f, 0.0f));
      ////mo.set_rotation(Vec3(1.0f, 0.0f, 0.0f), glm::radians(30.0f));
      //
      //   //set_rotation(Vec3(1.0f, 0.0f, 0.0f), glm::radians(30.0f));
      //model_instances[10]->set_transform(mo);
      //
      //mo.set_position(pos);
      //model_instances[10]->set_transform(mo);

      auto mo = model_instances[10];
      mo->transform( Transform::make_rotation(Vec3(1.0f, 0.0f, 0.0f), glm::radians(30.0f) ) );

      auto mo2 = model_instances[11];

      //auto t = Transform::make_translation(Vec3(0.10f, 0.0f, 0.0f));
      //auto r = Transform::make_rotation(Vec3(0.0f, 1.0f, 0.0f), glm::radians(30.0f));
      //r.look_at(Vec3(0.0, 0.0, 0.0), t.get_position());
      //t.set_rotation(r.get_rotation());
      auto mdl = mo2->model_transform;
      //mdl.look_at(Vec3(13.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.00001f) );
      static Float32 g = 0.0f;
      static Float32 h = 0.0f;


      static bool hm = true;
      static bool isPressed = false;
      static float time = ImGui::GetTime();
      float curTime = ImGui::GetTime();

      float timepast = curTime - time;

      if (glfwGetKey(window, GLFW_KEY_P)&& timepast > 0.25) {
         isPressed = !isPressed;


         time = ImGui::GetTime();
      }
      if (true && isPressed) {

         if (g > 2) {
            hm = false;
         }
         else if (g < -2) {
            hm = true;
         }

         if (hm) {
            g += 0.01f;
         }
         else {
            g -= 0.01f;
         }
         h += 0.01f;

         for (int i = 0; i < 64-1; i++) {
            Float32 size = 0.1f;
            //mo2->transform(Transform::make_translation(Vec3(0.0,0.0,0.0)));

            model_instances[i]->set_transform(Transform::make_translation(model_instances[i]->model_transform.get_position()));
            model_instances[i]->transform(Transform::make_rotation(Vec3((Float32)glm::cos(15 * g*(((i % 4))% 2)), (Float32)glm::cos(12*g*((i % 3))), (Float32)glm::sin(g/2* (i % 2)))) * Transform::make_translation(Vec3((Float32)glm::sin(g*i)*((i % 3)* size), (Float32)glm::sin(g* (i % 2)* size)*0.02 , (Float32)glm::sin(g/10)*((i%5) % 2))));

         }
         //model_instances[63]->set_transform(Transform::make_translation(model_instances[63]->model_transform.get_position()));
         auto cur = model_instances[63]->model_transform;
         //model_instances[63]->transform(Transform::make_translation(glm::normalize(Vec3(0.f, 0.f, 20.0f))));
         //model_instances[63]->transform(Transform::make_translation(glm::normalize(Vec3(1 * glm::cos(2 * g + cur.get_position().y) + cur.get_position().x, 1 * glm::sin(2 * g + cur.get_position().x) + cur.get_position().y, 0))));
         //model_instances[63]->transform(Transform::make_translation((Vec3((0.5 ) * glm::cos(2 * h + cur.get_position().x) /*+ cur.get_position().y*/, 0, (0.5 ) * glm::sin(2 * h /* + cur.get_position().y*/) /*+ cur.get_position().x*/))));
         Float32 radius = 30;
         Vec3 rotateAround(0.0,0.0,0.0);//Might be wrong
         Float32 speed = 5;
         model_instances[63]->set_transform(Transform::make_translation((Vec3((radius) * glm::cos(speed * h + rotateAround.x) + rotateAround.y, rotateAround.z, (radius) * glm::sin(speed * h  + rotateAround.x) + rotateAround.y))));

      }

      //mdl.set_position(Vec3(1.0f ,0.0f,0.0f));
      //mo2->transform(mdl);

      //mo2->set_transform(mdl);


      scene_manager.draw( view );

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
      glfwMakeContextCurrent( window );
      glfwSwapBuffers( window );


      //ImGui::Text( "\t%.1f FPS (avg %.3f ms/frame)",
        //          ImGui::GetIO().Framerate,
          //        1000.0f / ImGui::GetIO().Framerate

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
