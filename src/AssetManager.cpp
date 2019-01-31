#include "AssetManager.h"

SharedPtr<Model> AssetManager::loadModel(String const &filename) {
   if (_model_is_loaded[filename] && !_loaded_models[filename].expired()) {
      return _loaded_models[filename].lock(); // return the shared pointer made from the weak pointer
   }
   else {         
      String filenameAndDir = config::model_path + filename;
      auto model_ptr = std::make_shared<Model>(filenameAndDir);
      
      _model_is_loaded[filename] = true; // TODO: (låg prio) kommer aldrig att bli falsk; ej skalbart dynamiskt
      _loaded_models[filename] = WeakPtr<Model>(model_ptr);

      return model_ptr;
   }
}