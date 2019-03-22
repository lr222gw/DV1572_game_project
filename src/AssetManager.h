#pragma once

#include "misc/defs.h"
#include "Model.h"

#include "Config.h"

// TODO: G�r assetManager till en exclusiv konstruktionspunkt f�r model
//       Enda klassen som har tillg�ng till Models konstruktor...

class AssetManager {
public:
   // TODO: embed type into filename? extract from within file?
   [[nodiscard]] SharedPtr<Model> load_model( String const &filename );

private:
   // maps filenames to weak pointers
   HashMap<String, WeakPtr<Model>>   _loaded_models;
   HashMap<String, Bool>             _model_is_loaded;
};
