#include "misc/defs.h"

#include <GL/glew.h> // always include before gl.h & glfw3.h
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "misc/ImGui/imgui.h"
#include "misc/ImGui/imgui_impl_glfw.h"
#include "misc/ImGui/imgui_impl_opengl3.h"


/////////////////////////////////////////////////////////////////////

Str lowercase( char const *base ) {
   Str s( base );
   std::transform( s.begin(), s.end(), s.begin(), ::tolower );
   return s;
}

Str uppercase( char const *base ) {
   Str s = base;
   std::transform( s.begin(), s.end(), s.begin(), ::toupper );
   return s;
}

/////////////////////////////////////////////////////////////////////

MAKE_ENUM( Color, U8, red, green, blue, teal, banana, purple, pink, rainbow );


int main( int argc, char* argv[] ) {
   // initialise GLFW
   glewExperimental = true; // needed for core profile
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
   window = glfwCreateWindow( 1024, 768, "The Adventures of Rex II: Electric Bunghole [WORKING TITLE]", NULL, NULL );
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
   glClearColor( 0.3f, 0.3f, 0.3f, 0.0f );





   // Setup Dear ImGui context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO(); (void)io; // wot?
// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

   // setup Dear ImGui style
   ImGui::StyleColorsDark(); // ImGui::StyleColorsClassic();

   // setup Platform/Renderer bindings
   ImGui_ImplGlfw_InitForOpenGL( window, true );
   ImGui_ImplOpenGL3_Init( glsl_version );


    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);


   bool show_another_window = false;
   ImVec4 clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );

// Main loop:
   while ( !glfwWindowShouldClose(window) ) {
      // Poll and handle events (inputs, window resize, etc.)
      // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
      // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
      // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
      // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
      glfwPollEvents();

      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

       /* window listing all enum values of Color */ {
         ImGui::Begin( "Color enum values:" );

         Size count = 1;
         for ( auto  &e : Color::values() )
            ImGui::Text( "%-12s (color %zu/%zu)", e.to_string(), count++, Color::size() );

         ImGui::End();
      }

      // Rendering
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

   // Cleanup
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();


//   // main loop
//   do {
//      // clear screen
//      glClear( GL_COLOR_BUFFER_BIT );
//
//      // TODO: draw!
//
//      // swap buffers
//      glfwSwapBuffers( window );
//      glfwPollEvents();
//   }
//   while ( glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
//       && glfwWindowShouldClose(window) == 0 );

   glfwDestroyWindow( window );
   glfwTerminate(); // close OpenGL window & terminate GLFW

   return 0; // successful exit
}
