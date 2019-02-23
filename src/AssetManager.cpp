#include "AssetManager.h"

SharedPtr<Model> AssetManager::load_model( String const &filename ) {
   if ( _model_is_loaded[filename] && !_loaded_models[filename].expired() )
      return _loaded_models[filename].lock(); // return shared pointer made from weak pointer
   else {         
      String full_path = Config::model_path + filename;
      auto model_ptr   = std::make_shared<Model>( full_path );
      
      _model_is_loaded[filename] = true; // TODO: (low prio) will never reset; not scalable
      _loaded_models[filename]   = WeakPtr<Model>( model_ptr ); // register as weak pointer

      return model_ptr;
   }
}
