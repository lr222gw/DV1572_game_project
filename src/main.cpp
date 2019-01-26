#include "misc/defs.h"

#include <GL/glew.h> // <- always include before gl.h & glfw3.h
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "misc/ImGui/imgui.h"
#include "misc/ImGui/imgui_impl_glfw.h"
#include "misc/ImGui/imgui_impl_opengl3.h"


//Temporära bibliotek (och sånt) för temporära lösningar
#include "lop_Model.h"
//#include "main.h"

//#include <range/v3/all.hpp>

//En PreProcessor Macro						 ; #define		PI 3.14
//En vanlig Konstant är en "RunTime" konstant; const		int h_height
	//Känns igen bara till när programmet väl körs
//En "constexpr" är en "CompileTime" Konstant; constexpr	int h_height 
	//Känns igen till och med innan/under kompilerinsstadiet
//i.e. 
constexpr Int32	g_height = 768,
				g_width = 1024;


constexpr Float32	g_near_plane = 0.01f,
					g_far_plane = 100.0f,
					g_fov_rad = 0.01f;

/////////////////////////////////////////////////////////////////////


class Config {
public:
    StringView shader_path { "/dat/shader"; }
} g_config;



// OpenGL uses unsigned integers to keep track of
// created resources (shaders, vertices, textures, etc)
// For simplicity, we make them global here, but it is
// safe to put them in a class and pass around...
GLuint gVertexBuffer    = 0;
GLuint gVertexAttribute = 0;
GLuint gShaderProgram   = 0;
GLuint gUniformBuffer   = 0.0f;


struct ShaderData {
	glm::mat4 gRotate2D;
	float gOffsetX = 0.0f;
};
GLint gShaderDataLoc = -1;


/////////////////////////////////////////////////////////////////////



void create_shader() {

}


void CreateShaders() {
	




	// geometry shader:

		// create geometry shader "name" and store it in "gs"
	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);

	// open .glsl file and put it in a string
	shaderFile.open("GeometryShader.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	// glShaderSource requires a double pointer.
	// get the pointer to the c style string stored in the string object.
	shaderTextPtr = shaderText.c_str();

	// ask GL to use this string a shader code source
	glShaderSource(gs, 1, &shaderTextPtr, nullptr);

	// try to compile this shader source.
	glCompileShader(gs);

	// check for compilation error
	glGetShaderiv(gs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		memset(buff, 0, 1024);
		// query information about the compilation (nothing if compilation went fine!)
		glGetShaderInfoLog(gs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}


	// repeat process for Fragment Shader (or Pixel Shader)
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open("FragmentShader.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);
	// query information about the compilation (nothing if compilation went fine!)
	compileResult = GL_FALSE;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetShaderInfoLog(fs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	// link shader program (connect vs and ps)
	gShaderProgram = glCreateProgram();
	glAttachShader(gShaderProgram, fs);
	glAttachShader(gShaderProgram, gs);
	glAttachShader(gShaderProgram, vs);
	glLinkProgram(gShaderProgram);

	// check once more, if the Vertex Shader and the Fragment Shader can be used
	// together
	compileResult = GL_FALSE;
	glGetProgramiv(gShaderProgram, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetProgramInfoLog(gShaderProgram, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}
	// in any case (compile sucess or not), we only want to keep the
	// Program around, not the shaders.
	glDetachShader(gShaderProgram, vs);
	glDetachShader(gShaderProgram, gs);
	glDetachShader(gShaderProgram, fs);
	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);
}

void CreateUniformBuffer() {
	glGenBuffers(1, &gUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, gUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderData), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_UNIFORM_BUFFER, 10, gUniformBuffer);
}

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
      glClearColor( 0.45f, 0.55f, 0.60f, 1.00f );
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


                        

class IEntity {
public:
   using EntityID = Uint64;

   IEntity():
      _id ( _generate_id() ) // assign unique ID
   {}

   virtual void update( Float32 delta_t ) = 0;
   virtual void render( Float32 delta_t ) = 0;

   EntityID get_id() const { return _id };

private:
   // generate unique ID
   [[nodiscarð]]  EntityID _generate_id() {
      static EntityID next_id { 1 };
      return next_id++;
  }

  EntityID _id;
};






UnorderedMap<EntityID, String> entity_name;
entity_name[id];




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
   Optional<GraphNode> find( EntityID id ) {
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










MAKE_ENUM( ShaderType, Uint8, vertex, geometry, fragment ); // TODO: add more shaders here AND in get_type

class ShaderHandler {
public:
    using ShaderID = Uint64; // 64-bit representation used to representate shaders with unique IDs.

// TODO: embed type into filename? extract from within file?
[[nodiscarð]] ShaderID add_shader( StringView filename  ) {
   if ( _shader_ids.contains(filename) )
      return _shader_ids[filename]; // returnera ID:t till befintlig instans
   else {
      //------------------------------------FIRST READ THE SHADER FROM FILE---------------------------//
      // declare files we need
      String        shader_code;
      Ifstream      shader_file;
      StringStream  shader_stream;

      // .vs = vertex   shader
      // .gs = geometry shader
      // .fs = fragment shader
      ShaderType type = _extract_type( filename );

      // ensure ifstream objects can throw exceptions:
      shader_file.exceptions( std::ifstream::failbit | std::ifstream::badbit );

      try {
         shader_file.open( g_config.shader_path + filename ); // open file
         shader_stream << shader_file.rdbuf();                // read content into stream
         shader_file.close();                                 // close file handle
         shader_code = shader_stream.str();                   // convert to string
      }
      catch ( std::ifstream::failure e ) {
         std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
      }
      const char* shader_code_str = shader_stream.c_str();

      //-------------------------------------THEN COMPILE THE SHADER----------------------------------//

      // local buffer to store error strings when compiling.
      char buffer[1024];
      memset( buffer, 0, 1024 );
      GLint compile_result = 0;

      // vertex shader:
      // create vertex shader "name" and store it in "vs"
      GLuint location;

      switch ( type ) {
         case ShaderType::vertex:   location = glCreateShader( GL_VERTEX_SHADER   ); break;
         case ShaderType::geometry: location = glCreateShader( GL_GEOMETRY_SHADER ); break;
         case ShaderType::fragment: location = glCreateShader( GL_FRAGMENT_SHADER ); break;
         default: throw {}; // TODO: exception?
      }
      // glShaderSource requires a double pointer.
      // get the pointer to the c style string stored in the string object.
      const char* shaderTextPtr = shaderText.c_str();

      // ask GL to use this string a shader code source
      glShaderSource( location, 1, &shader_code_str, nullptr );

      // try to compile this shader source.
      glCompileShader( location );

      // check for compilation error
      glGetShaderiv( location, GL_COMPILE_STATUS, &compile_result );
      if ( compile_result == GL_FALSE ) {
         // query information about the compilation (nothing if compilation went fine!)
         glGetShaderInfoLog( location, 1024, nullptr, buffer );
         // print to Visual Studio debug console output
         OutputDebugStringA( buffer );
      }

      //----------------------------------------THEN RETURN-------------------------------------------//

      // om filen laddats in utan problem:
      ShaderID id   = _generate_id( type ); // generate unique ID
      _shaders[id]  = shader_code; // TODO: <LOW PRIORITY> move semantics optimization (emplace?)
      return id;
   }
}

[[nodiscarð]] Shader const & get_shader( ShaderID id ) const {
   if ( _shaders.contains(id) )
      return _shaders[id];
   else throw {}; // TODO: exceptions
}

// extracts the type of a shader from a shader ID
// by extracting the 8 leftmost bits and converting to ShaderType enum type.
[[nodiscarð]]  ShaderType get_type( ShaderID id ) const {
    return static_cast<ShaderType>(id >> 56); // TODO: if it doesn't work, use dynamic_cast
}

private:
  // Maintains a static ID counter that determines the next ID.
  // Embeds the shader type category into the ID by masking the 8 leftmost bits.
  [[nodiscarð]]  ShaderID _generate_id( ShaderType type ) {
      static ShaderID next_id { 1 };
      return (type << 56) & next_id++;
  }
  
  [[nodiscard]] ShaderType _extract_type( StringView filename ) const {
      auto extension = filename.substr(filename.find_last_of(".") + 1);
      if      ( extension == "vs" )
          return ShaderType::vertex;
      else if ( extension == "gs" )
          return ShaderType::geometry;
      else if ( extension == "fs" )
          return ShaderType::fragment;
      else throw {}; // TODO: exceptions
  }
    
  UnorderedMap<ShaderID,Shader> _shaders;    // maps unique shader IDs to loaded shaders. 
  UnorderedMap<String,ShaderID> _shader_ids; // maps shader filenames to unique shader IDs
};













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
};


	
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

// generates a 4x4 world matrix
inline glm::mat4  generate_model_matrix() {
	return  glm::mat4{ 1.0f }; // 4x4 identity matrix
}


// generates a 4x4 view matrix
inline glm::mat4  generate_view_matrix
(   // function args:
	glm::vec3  camera_pos	 = { 0.0f,  0.0f, -2.0f },
	glm::vec3  camera_target = { 0.0f,  0.0f,  0.0f },
	glm::vec3  camera_up_vec = { 0.0f,  1.0f,  0.0f }
) { // function body
	return glm::lookAt(camera_pos, camera_target, camera_up_vec);
}


// generates a 4x4 perspective matrix
inline glm::mat4  generate_perspective_matrix
(   // function args:
	Float32  near_plane	= g_near_plane,
	Float32  far_plane	= g_far_plane,
	Float32  fov_rad	= g_fov_rad
) { // function body
	Float32  aspect = (Float32)g_width / (Float32)g_height;
	return glm::perspective(fov_rad, aspect, near_plane, far_plane);
}



Int32 main(Int32 argc, char* argv[]) {
	// initialise GLFW
	glewExperimental = true; // <- needed for core profile
	if (!glfwInit()) {
		fprintf( stderr, "[ERROR] Failed to initialize GLFW.\n" );
		return -1;
	}

	// 4xAA
	glfwWindowHint( GLFW_SAMPLES, 4 );
	// GLSL v130
	const char* glsl_version = "#version 440";
	// OpenGL v4.4
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 4 );
	// for MacOS; should not be needed
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	// use OpenGL core profile
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	// open a window and create its OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow(g_width, g_height, "3D Project -- WINDOW", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "[ERROR] Failed to open GLFW window.\n"
			"        If you have an Intel GPU, they're not 4.4 compatible.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // initialize GLEW
 // glfwSwapInterval(1); // enable vsync -- TODO: decide!

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "[ERROR] Failed to initialize GLEW.\n");
		return -1;
	}

	// ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// dark grey background
	glClearColor(0.2f, 0.2f, 0.2f, 0.2f);

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

	CreateShaders();
	CreateUniformBuffer();



	//Test for ModelLoading, Early testing of Deffered Rendering
	Model modelObj{ "dat/meshes/12330_Statue_v1_L2.obj" };
	
	//Camera
	CameraData cam = make_camera();

	Renderer renderer(std::move(cam));

	//Aktivera DepthTest och StencilTest för OpenGL...
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
 // Main loop:
	while (!glfwWindowShouldClose(window)) {
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
		Int32 display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		float dt_time_s = ImGui::GetIO().DeltaTime;

		glBindBuffer(GL_UNIFORM_BUFFER, gUniformBuffer);
		Render();
		

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