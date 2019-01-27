//-------------------SHADERS----------------------------//

MAKE_ENUM( ShaderType,...);

class Shader {
   Shader( char const *code, ShaderType type ); // skapa via ShaderManager::load_shader(...);
   ~Shader();
};

class ShaderProgram {
   ShaderProgram( Vector<SharedPtr<Shader>> &&shaders );
   ~ShaderProgram();
};

class ShaderManager {
   load_shader( StringView filename );
};

//-------------------MODELS----------------------------//

class Model { // skapa via AssetManager::load_model(...)
public:
   // destructor (TODO: vad ska vara i destruktorn?)
   // ...
private:

   // private constructor + friend class AssetManager?  (TODO: vad ska vara i konstruktorn?)
};



class ModelInstance { // skapa via SceneManager::add(...)
public:
   // destructor  (TODO: vad ska vara i destruktorn?)
   // ...
   // set/get position
   // render
private:
   // private constructor + friend class SceneManager? (TODO: vad ska vara i konstruktorn?)
   SharedPtr<Model>  _model;
   glm::vec3         _global_position;
   // unique ID?
}; 


class AssetManager {
   SharedPtr<Model> load_model( StringView filename );
};

//-------------------SCENE----------------------------//

class SceneManager {
   add( SharedPtr<Model> model_instance, Vec3 global_pos );
   void update();
   void render();
// TODO: scene graph, attaching nodes, pruning, transforms
};

void init_demo_scene(...);  // skapar vår demoscen

//-------------------ENGINE----------------------------//

int main(...) {
   // init openGL
   // init imgui
   // skapa scene manager
   // skapa asset manager
   // main loop
}