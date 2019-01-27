#pragma once

class AssetManager; <- funktion: ladda in modeller (returnerar ett handle)

class SceneManager; <- funktion: instantiera modeller som laddats in via AssetManager samt rendrera?

// KOMMENTAR: allt detta ska flyttas in i respektive headers och source filer

class ModelInstance {
public:
   ModelInstance( SharedPtr<Model> model, Vec3 global_position ):
      _model            ( model ),
      _global_position  ( global_position )
   {}

   void render( Float32 delta_t ) {
      // TODO:
      //    måste rendrera via _model 
      //    men ta hänsyn till instansens attribut (t.ex. position)
   }

   void set_position( Vec3 new_global_position ) {
      _global_position = new_global_position;
      // TODO:
      //    framför ändringen till shadern?
   }

/* kommentarer:
      ett context som parameter? (vart ska vi rendrera instansen)
*/

private:
   SharedPtr<Model> _model; // från AssetManager::load_model(...);


   struct TransformData {
      Mat4 transform; // rotation, skala, translation
   };


// TODO: flytta in transform i vår SceneManager
//  Vec3             _global_position;
// och senare:
// ???              _rotation;
// Float32          _scale;
// Bool             _is_hidden;
};



class Entity {
   void update( Float32 delta_t ) {
      // räkna ut en Mat4 med delta_t
      manager->transform_instance( my_id, my_transform );
   };
private:
   SceneManager *manager;
};


class SceneManager {
   void render() {
      // TODO: fundera över maxkapacitet hos överföringar till GPUn
      for ( Uint32 i = 0;  i < _instance_nr;  ++i )
         _models[i].render( _transforms[i] );
   }

   void transform_instance( Index instance_id, Mat4 transform ) {
      _transforms[instance_id] *= transform;
   }

   Vector<SharedPtr<Model>>  _models;
   Vector<Mat4>              _transforms; // <-- våra transformer

   Uint32                    _instance_nr;
}








class SceneManager {
public:
   SharedPtr<ModelInstance> instantiate( SharedPtr<Model> model, Mat3 global_position ) {
      // TOOD:
      //    create a ModelInstance of the model at the position
      //    add to _instances
      //    return sharedptr to it
   }

   void render( Float32 delta_t ) {
      for ( auto &e : _isntances )
         e.render( delta_t );
   }

private:
// Scene *_current_scene;
   Vector<ModelInstance> _instances;
}
