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


void processInput(GLFWwindow *window);

//#include "misc/stb_image.h" //TODO: ska denna  vara här?

//Temporära bibliotek (och sånt) för temporära lösningar
//#include "lop_Model.h"
//#include "main.h"

//#include <range/v3/all.hpp>

//En PreProcessor Macro						 ; #define		PI 3.14
//En vanlig Konstant är en "RunTime" konstant; const		int h_height
	//Känns igen bara till när programmet väl körs
//En "constexpr" är en "CompileTime" Konstant; constexpr	int h_height 
	//Känns igen till och med innan/under kompilerinsstadiet
//i.e. 





/////////////////////////////////////////////////////////////////////


   // OpenGL uses unsigned integers to keep track of
   // created resources (shaders, vertices, textures, etc)
   // For simplicity, we make them global here, but it is
   // safe to put them in a class and pass around...
   // TODO: fundera över vart dessa borde bo?
//   GLuint g_vertex_buffer    = 0;
//   GLuint g_vertex_attribute = 0;
//   GLuint g_shader_program   = 0; // TODO: ta bort -- då vi ska hantera den med klassen ShaderProgram och skicka den som argument
//   GLuint g_uniform_buffer   = 0;
//




/////////////////////////////////////////////////////////////////////
/*
TODO:   fundera över om vi ska ha en Scene-klass och på så vis tillåta
        att växla scener senare via SceneManager. (LÅG PRIORITET)

class Scene {

// function that loads the test scene
Scene( AssetManager &assets, SceneManager &scene ):
   _assets ( assets ),
   _scene  ( scene  )
{
   // load the statue into memory
   auto statue_handle = assets.load_model( "Statue" );

   // create 9 statues arranged thusly:
   //   1  2  3    
   //   4  5  6    
   //   7  8  9    
   for ( int i=0;  i<9;  ++i ) {
      glm::vec3 position { i/3, i%3, 0 };
      auto instance_id = scene.add_model( statue_handle, position ); // 
      instances.push_back( instance_id );
   }
}

private:
   AssetManager  &_assets;
   SceneManager  &_scene;
};*/













/*
class Model {};

class Renderer {
public:

private:
   UnorderedMap<ModelId, ModelInstance> _instances;
}*/




/*
void create_uniform_buffers() {
	glGenBuffers( 1, &g_uniform_buffer );
	glBindBuffer( GL_UNIFORM_BUFFER, g_uniform_buffer );
	glBufferData( GL_UNIFORM_BUFFER, sizeof(ShaderData), NULL, GL_DYNAMIC_COPY );
	glBindBufferBase(GL_UNIFORM_BUFFER, 10, g_uniform_buffer );
}
*/


/*
CameraData make_camera() {

   //initiserar Model (world) matrisen
   glm::mat4  model = generate_world_matrix();

   //Initierar View-matrisen
   glm::mat4  view = generate_view_matrix();
   //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

   //Float32 FOV = glm::radians(45.0f);
   //Float32 aspectRatio = g_height / g_width;
   glm::mat4  projection = generate_perspective_matrix();//;glm::perspective(FOV, aspectRatio, 0.1f, 100.0f);

   //CameraData camera{view, model, projection};
   CameraData camera;
   camera.view = view;
   camera.model = model;
   camera.projection = projection;

   return camera;

}
*/


/*
TODO:  fundera över hur vi ska hantera viewport/camera i vårt projekt
class Viewport {
public:

   Viewport():
      _view        ( generate_view_matrix()        ),
      _model       ( generate_model_matrix()       ),
      _perspective ( generate_perspective_matrix() )
   {}


   // Temporär funktion;
   // ersätts av render i modellerna efter att Assimp integrerats
   // TODO: migrera globala g_shader till någonstans
   void render() {
      glClearColor( 0.2f, 0.2f, 0.2f, 0.2f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear color & Z buffer

      glUseProgram( g_shader );

      // pass uniforms
      glUniform3fv(   gUniformTintColorLoc, 1, &gUniformTintColor[0] );
      glUniform1fv( gUniformBlendFactorLoc, 1, &gUniformBlendFactor  );

      model_loc       = glGetUniformLocation( g_shader, "modelMatrix" );
      view_loc        = glGetUniformLocation( g_shader, "viewMatrix" );
      perspective_loc = glGetUniformLocation( g_shader, "perspectiveMatrix" );

      glUniformMatrix4fv(       model_loc, 1, GL_FALSE, glm::value_ptr(_model)       );
      glUniformMatrix4fv(        view_loc, 1, GL_FALSE, glm::value_ptr(_view)        );
      glUniformMatrix4fv( perspective_loc, 1, GL_FALSE, glm::value_ptr(_perspective) );

      // tell opengl we are going to use the VAO we described earlier
      glBindVertexArray( gVertexAttribute );

      // ask OpenGL to draw 6 vertices starting from index 0 in the vertex array 
      // currently bound (VAO), with current in-use shader. Use TOPOLOGY GL_TRIANGLES,
      // so for one quad we need 6 vertices (two tris)
      glDrawArrays( GL_TRIANGLES, 0, 6 );
   }

   bool add_shader( Shader &&s ) {}

private:
	glm::mat4	_view,
				   _model,
				   _projection;
   GLint       _view_loc,
               _model_loc,
               _perspective_loc;

   Vector<Shader> _shaders;

};
*/


                        
/*
class IEntity {
public:
   using EntityId = Uint64;

   IEntity():
      _id ( _generate_id() ) // assign unique Id
   {}

   virtual void update( Float32 delta_t ) = 0;
   virtual void render( Float32 delta_t ) = 0;

   EntityId get_id() const { return _id };

private:
   // generate unique Id
   [[nodiscard]]  EntityId _generate_id() {
      static EntityId next_id { 1 };
      return next_id++;
  }

  EntityId _id;
};*/





/*
HashMap<EntityId, String> entity_name;
entity_name[id];*/



/*
class GraphNode : public IEntity {
public:
   GraphNode( IEntity &e ):
      _element  ( e )
   {}

   void update( Float32 delta_t ) {
      _element.update( delta_t );
      for ( auto &c : _children )
         c.update( delta_t );
   }

   void render( Float32 delta_t ) {
      _element.render( delta_t );
      for ( auto &c : _children )
         c.render( delta_t );
   }

   void attach( IEntity &e ) {
      _children.push_back( GraphNode(e) ); // TODO: <LOW PRIORITY> emplace_back? optimisering
   }

   void detach( IEntity const &e ) {
      std::remove( _children.begin(), _children.end(), e );
   }

   Vector<GraphNode> & get_children() {
      return _children;
   }

   IEntity const & get_element() const {
      return _element;
   }

   //  depth first search
   Optional<GraphNode> find( EntityId id ) {
      for ( auto &c : _children ) {
         if ( id == c.get_id() )
            return c;
         else {
            Optional<GraphNode> match = c.find(id);
            if ( match ) // match was found
               return *match;
         }
      }
      return {}; // no match was found
   }

private:
   IEntity            &_element;
   Vector<GraphNode>   _children;
};
*/

/*

main() {
   1. initiera allt som har med openGL, imgui, osv att göra

   2. skapa viewport
   3. skapa scene graph
   4. ladda några element (t.ex. meshen) in i scene graphen

   loop:
      uppdatera & rendrera allt i scene graph (relativt till viewport?)

   x: avsluta allt
}


*/








void create_demo_scene( /*AssetHandler &assets, Scene &scene, RenderHandler &renderer*/ ) {
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

// generates a 4x4 world matrix
inline glm::mat4  generate_model_matrix() {
   return  glm::mat4{ 1.0f }; // 4x4 identity matrix
}


// generates a 4x4 view matrix
inline glm::mat4  generate_view_matrix
(   // function args:
   glm::vec3  camera_pos    = { 0.0f,  0.0f, -2.0f },
   glm::vec3  camera_target = { 0.0f,  0.0f,  0.0f },
   glm::vec3  camera_up_vec = { 0.0f,  1.0f,  0.0f }
) { // function body
   
   return glm::lookAt(camera_pos, camera_target, camera_up_vec);
}

// generates a 4x4 perspective matrix
inline glm::mat4  generate_perspective_matrix
(   // function args:
   Float32  near_plane = config::near_plane,
   Float32  far_plane  = config::far_plane,
   Float32  fov_rad    = config::fov_rad
) { // function body
   Float32  aspect = (Float32)config::width / (Float32)config::height;
   return glm::perspective(config::fov_rad, aspect, config::near_plane, config::far_plane);
}
 



/*
// alternativ till scene graph som verkar vara mer förekommande och korrekt
class RenderHandler {
public:
   void update() {
      for ( auto &e : loaded_entities )
         e.update();
   }

   void render() {
      for ( auto &e : loaded_entities )
         e.render();
   }

   void load( IEntity e ) {  };

private:
   Vector<IEntityInstance>  loaded_entities;
};
*/






























/*
class Renderer {
public:
	// && => temporärt värde; kallas "rValueReference", vi kan "stjäla" värdet
	Renderer(CameraData &&cd) : _camera(std::move(cd)) {}
						// "&&" indikerar vi att det är ett temporärt värde... 
						// Försvinner efter funktionanropet
			 // Med "std::move(cd)" så "Klipper vi ut" datan till "cd"-parametern
			 // Det innebär att om vi skickar in en "CameraData objekt" till 
			 // denna konstruktor såhär "renderer(std::move(myCamData))" Så
			 // Kommer inte "myCamData" kunna användas mer efter vi skapat 
			 // "Renderer"-objektet  genom denna konstruktor.
			 // Datan hamnar Automatiskt i "_camera" då det är den som följer 
			 // "Render(CameraData &&cd)" med ": _camera(std::move(cd))" 
					// "_camera" <- detta är en referering av vårt privata Attribut "_camera"
			  
			//När ":" följ+s efter en Konstruktor påbörjas vad som kallas en 
			// "initialiser List", där vi initiserar Attributen genom 
				//Attributnamnet + en Parentes med datan attributet ska innehålla

	//https://www.geeksforgeeks.org/when-do-we-use-initializer-list-in-c/
	// Vi använder "Initializer List" då vi nu får alla 
	// Definitioner skilda från alla Deklarationer; 
			//Deklarationer sker nu alltid i Konstruktorn
			//Attribut deklareras inte i deras defintioner längre... 
	// En annan anledning är; Om vi har ett attribut som är en Konstant Pekare; "obj const &father"
	// så kan vi sätta ett värde på den i "Initlializer List" baserat på vad vi skickar 
	// till vår konstruktor...

	//Olika typer av Konstruktorer
		//Klassnamn(Klassnamn  const &other) {}				// copy constructor
		//Klassnamn(Klassnamn  &&other) {}					// move constructor
		//Klassnamn & operator=(Klassnamn const &other) {}	// copy assignment
		//Klassnamn & operator=( Klassnamn &&other ) {}		// move assignment
		//Klassnamn(Klassnamn  &&other) = delete;			// generera inte en copy constructor, 
																//dumma kompilator.
	inline void update(Float32 delta_t) {

	}

private:
	CameraData _camera;
};*/


	
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

	//Shaders, Uniform hantering, 

	// Camera
//	CameraData cam = make_camera();
//	Renderer renderer(std::move(cam));


   //Gör Instans av ShaderManager
      //Följande kod skrivs här, men ska brytas ut till en lämpligare placering
         //Ladda in Shaders
         //Gör ShaderProgram 
         //Knyt ShaderProgram till OpenGL

   //Gör Instans av SceneManager
   //Gör Instans av AssetManager

	// activate depth test and stencil test for openGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

   //testa   vår AssetManager TODO: heh 
   ShaderManager shaMan{};
   auto fraShader = shaMan.load_shader("fraSha.fs");
   auto vertShader = shaMan.load_shader("vertSha.vs");

   auto shaProg = shaMan.create_program({fraShader, vertShader});

   AssetManager assMan{};
   SharedPtr<Model> myModel = assMan.load_model("Pokemon.obj");
   
   SceneManager scenMan{};
   SharedPtr<ModelInstance> modelInstance = 
      scenMan.instantiate_model(myModel,
                                shaProg, 
                                Vec3(0.0f, 0.0f, 1.0f));
   //scenMan

   Vec3 cam_rotations { 0.0f, 0.0f,  0.0f };
   Vec3 cam_positions { 0.0f, 0.0f, 30.0f };
   Float32 fov_rad = config::fov_rad; // 90 degrees

   Viewport myView { cam_positions, fov_rad };
   myView.bind_shader_program(*shaProg);
   
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



 // main loop:
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
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

      myView.bind_shader_program(*shaProg);

      draw_camera_debug_window( cam_positions, cam_rotations, fov_rad );
      myView.set_rotation( cam_rotations );
      myView.set_position( cam_positions );
      myView.set_fov( fov_rad );

      
		glClearColor(1.2, 0.2f, 0.2f, 1.0f);//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
      
// programkod här
      //myMinstance->ttranceform(aTransformMatris)
      //myMinstance->render(&*shaProg);

//glm::mat4 projection = glm::perspective(fov_rad, (float)config::width / (float)config::height, 0.1f, 100.0f);
//glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 30.0f), glm::vec3(0.0f, 0.0f, 30.0f) + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//glUniformMatrix4fv(glGetUniformLocation(shaProg->getProgramLoc(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//glUniformMatrix4fv(glGetUniformLocation(shaProg->getProgramLoc(), "view"), 1, GL_FALSE, glm::value_ptr(view));
//glm::mat4 model;
//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Translate it down a bit so it's at the center of the scene
//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// It's a bit too big for our scene, so scale it down
//glUniformMatrix4fv(glGetUniformLocation(shaProg->getProgramLoc(), "model"), 1, GL_FALSE, glm::value_ptr(model));

      //myModel->draw(*shaProg);
      
      
      scenMan.draw(); // undersök om buffer binds
      //glUseProgram(shaProg->getProgramLoc());
      //a_Mesh.render();


	  ImGui::Render();
	  // !! ImGui kod här !!

	  // rendering

	  Int32 display_w, display_h;
	  glfwMakeContextCurrent(window);
	  glfwGetFramebufferSize(window, &display_w, &display_h);
	  glViewport(0, 0, display_w, display_h);
	  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	//	float dt_time_s = ImGui::GetIO().DeltaTime; // UNUSED

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}
	// main loop end

	// cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate(); // close OpenGL window & terminate GLFW

	return 0; // successful exit   

}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}