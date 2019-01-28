#pragma once

#include "misc/defs.h"
#include "lop_Model.h"
#include "Config.h"

class AssetManager {
public:
   // TODO: embed type into filename? extract from within file?
   [[nodiscard]] SharedPtr<Model> load_model(String const &filename);   

private:
   /*
   UnorderedMap<ShaderProgramId,GLuint>  _shader_programs; // maps unique shader Ids to loaded shaders.
   UnorderedMap<ShaderId,GLuint>         _shaders;         // maps unique shader Ids to loaded shaders.
   */
   HashMap<String, WeakPtr<Model>>   _loaded_models;         // maps filenames to weak pointers
   HashMap<String, Bool>             _model_is_loaded;


};


