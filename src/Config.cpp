#include "Config.h"

String const Config::shader_path  { "dat/shaders/" };
String const Config::model_path   { "dat/models/"  };
String const Config::texture_path { "dat/models/"  }; // TODO: currently unneeded

#ifdef _DEBUG
   Bool Config::should_draw_debug_elements = true;
#endif /*DEBUG*/

Config config {}; // <- definition of global config instance
