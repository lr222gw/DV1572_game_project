#pragma once

#include "misc/defs.h"
#include "Model.h"
#include "Transform.h"

#include "misc/ImGui/imgui.h"
#include "misc/ImGui/imgui_impl_glfw.h"
#include "misc/ImGui/imgui_impl_opengl3.h"

class ModelInstance {
public:
   ModelInstance( SharedPtr<Model> model,
                  SharedPtr<ShaderProgram> shader_program,
                  Transform const &transform)
   :
      _model          ( model ),
      _shader_program ( shader_program ),
      _transform      ( transform ),
      model_transform ( _transform )
   {}
   void draw();
   void transform(     Transform const &transform ); // TODO: move semantics
   void set_transform( Transform const &transform ); // TODO: move semantics

   SharedPtr<ShaderProgram>  get_shader_program(); // TODO: move semantics
   void set_shader_program(SharedPtr<ShaderProgram> shaderprogram); // TODO: move semantics

   [[nodiscard]] SharedPtr<Model const> get_model() const;

private:
   SharedPtr<Model>          _model;
   SharedPtr<ShaderProgram>  _shader_program;
   Transform                 _transform;

public:
   Transform const &model_transform; // read-only exposure
};







//class AssetManager; < -funktion: ladda in modeller(returnerar ett handle)
//
//class SceneManager; < -funktion: instantiera modeller som laddats in via AssetManager samt rendrera ?
//
////Todo;
//   //-Implementera AssetManager, Se till att Importering av Modell-filer genom Assimp Fungerar (Insperation kolla på Shadermanager)
//         //LoadModel()
//   //-Implementera Model-klassen
//         //Mesh    Struct
//         //Texture Struct
//
//   //ViewPort Klassen; Render Via Scene, skicka in "view"
//         //Viewport view;
//         //SceneManager scene;
//         //
//         //main loopen:
//         //while (...) {
//         //   auto delta_t = ...;
//         //   scene.render(view, delta_t);
//         //}
//
//class AssetManager {
//public:
//   [[nodiscard]] SharedPtr<Model>   load_model(String const &);
//
//private:
//   HashMap<String, WeakPtr<Model>>  _loaded_models;
//   HashMap<String, Bool>            _model_is_loaded;
//};
//
//
//
//// KOMMENTAR: allt detta ska flyttas in i respektive headers och source filer
//
//class ModelInstance {
//public:
//   ModelInstance( SharedPtr<Model> model, Vec3 global_position ):
//      _model            ( model ),
//      _global_position  ( global_position )
//   {}
//
//   void render( Float32 delta_t ) {
//      // TODO:
//      //    måste rendrera via _model
//      //    men ta hänsyn till instansens attribut (t.ex. position)
//   }
//
//   void transform(Mat4) {//Multiplicera med Camera Matriserna  }
//
///* kommentarer:
//      ett context som parameter? (vart ska vi rendrera instansen)
//*/
//
//private:
//   //  Mat4              _instance_transform; // rotation, scale, translation
//   InstanceData      _data; // rotation, scale, translation
//   SharedPtr<Model> _model; // från AssetManager::load_model(...);
//
//
//   struct TransformData {
//      Mat4 transform; // rotation, skala, translation
//   };
//
//
//// TODO: flytta in transform i vår SceneManager
////  Vec3             _global_position;
//// och senare:
//// ???              _rotation;
//// Float32          _scale;
//// Bool             _is_hidden;
//};
//
//struct Context {
//   //Scene *scene;           //Scen och SceneManager, krävs båda? Eller ska vi bara ha scenemanager?
//   SceneManager *scene;
//   AssetManager *assets;
//};
//
//struct InstanceData {
//   Vec3  global_position,
//      // rotation, scale, ....
//};
//
//class GObject {
//public:
//   GObject(Context &context, String const &model_filename, InstanceData params) :
//      _context(context)
//   {
//      auto model = _context.assets.load(model_filename);
//      _context.scene.add(model, params);
//   }
//
//   virtual ~GObject() {
//      _context.scene.remove(this);
//   }
//
//private:
//   Context &_context;
//};
//
//   // TODO: (LOW PRIORITY)
//   //class Player : public GObject {
//   //public:
//   //   Player(Context &context, String const &model_name, InstanceData params) : GObject(context, model_name, params) {
//   //      // ...
//   //   }
//   //   // ....
//   //};
//
////class Entity {
////   void update( Float32 delta_t ) {
////      // räkna ut en Mat4 med delta_t
////      manager->transform_instance( my_id, my_transform );
////   };
////private:
////   SceneManager *manager;
////};
//
//
//class SceneManager {
//   void render() {
//      // TODO: fundera över maxkapacitet hos överföringar till GPUn
//      for ( Uint32 i = 0;  i < _instance_nr;  ++i )
//         _models[i].render( _transforms[i] );
//   }
//
//   void transform_instance( Index instance_id, Mat4 transform ) {
//      _transforms[instance_id] *= transform;
//   }
//
//   Vector<SharedPtr<Model>>  _models;
//   Vector<Mat4>              _transforms; // <-- våra transformer
//
//   Uint32                    _instance_nr;
//}
//
//
//
//
//
//class SceneManager {
//public:
//   SharedPtr<ModelInstance> instantiate( SharedPtr<Model> model, Mat3 global_position ) {
//      // TOOD:
//      //    create a ModelInstance of the model at the position
//      //    add to _instances
//      //    return sharedptr to it
//   }
//
//   void render( Float32 delta_t ) {
//      for ( auto &e : _isntances )
//         e.render( delta_t );
//   }
//
//private:
//// Scene *_current_scene;
//   Vector<ModelInstance> _instances;
//}
//
