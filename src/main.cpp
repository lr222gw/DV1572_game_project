#include "misc/defs.h"

#include <GL/glew.h> // <- always include before gl.h & glfw3.h
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "misc/ImGui/imgui.h"
#include "misc/ImGui/imgui_impl_glfw.h"
#include "misc/ImGui/imgui_impl_opengl3.h"



#include "lop_Model.h"
#include "main.h"

//#include <range/v3/all.hpp>

//En konstant
constexpr int	g_height = 768,
				g_width = 1024;


/////////////////////////////////////////////////////////////////////
void init_camera();

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

/////////////////////////////////////////////////////////////////////


int main( int argc, char* argv[] ) {
   // initialise GLFW
   glewExperimental = true; // <- needed for core profile
   if ( !glfwInit() ) {
      fprintf( stderr, "[ERROR] Failed to initialize GLFW.\n" );
      return -1;
   }

   // 4xAA
   glfwWindowHint( GLFW_SAMPLES, 4 );
   // GLSL v130
   const char* glsl_version = "#version 130";
   // OpenGL v3.3
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
   // for MacOS; should not be needed
   glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
   // use OpenGL core profile
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

   // open a window and create its OpenGL context
   GLFWwindow* window;
   window = glfwCreateWindow(g_width, g_height, "3D Project -- WINDOW", NULL, NULL );
   if ( window == NULL ) {
      fprintf( stderr, "[ERROR] Failed to open GLFW window.\n"
                       "        If you have an Intel GPU, they're not 3.3 compatible.\n" );
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
   glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE ) ;

   // dark grey background
   glClearColor( 0.2f, 0.2f, 0.2f, 0.2f );

   // ImGui context setup
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO(); (void)io; // wot?
// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

   // setup ImGui style
   ImGui::StyleColorsDark();

   // setup platform and renderer bindings
   ImGui_ImplGlfw_InitForOpenGL( window, true );
   ImGui_ImplOpenGL3_Init( glsl_version );

   // clear color
   ImVec4 clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );

   //Camera
   init_camera();


   //Test for ModelLoading, Early testing of Deffered Rendering
   Model hehe { "dat/meshes/12330_Statue_v1_L2.obj" };

   //Model myModel = Model(hehe);


// Main loop:
   while ( !glfwWindowShouldClose(window) ) {
      // Poll and handle events (inputs, window resize, etc.)
      // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
      // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
      // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
      // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
      glfwPollEvents();

      // start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // !! ImGui kod här !!
      
      // rendering
      ImGui::Render();
      int display_w, display_h;
      glfwMakeContextCurrent( window);
      glfwGetFramebufferSize( window, &display_w, &display_h );
      glViewport( 0, 0, display_w, display_h );
      glClearColor( clear_color.x, clear_color.y, clear_color.z, clear_color.w );
      glClear( GL_COLOR_BUFFER_BIT );
      ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

      glfwMakeContextCurrent(window);
      glfwSwapBuffers(window);
   }
// main loop end

   // cleanup
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   glfwDestroyWindow( window );
   glfwTerminate(); // close OpenGL window & terminate GLFW

   return 0; // successful exit
}

void init_camera() {

	glm::mat4  model{ 1.0f };
	glm::mat4  view{ 1.0f };

	float FOV = glm::radians(45.0f);
	float aspectRatio = g_height / g_width;
	glm::mat4  projection = glm::perspective(FOV, aspectRatio, 0.1f, 100.0f);

	




}