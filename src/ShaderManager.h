#pragma once

#include "misc/defs.h"
#include "Shader.h"
#include "ShaderProgram.h"

class ShaderManager {
public:
   // TODO: embed type into filename? extract from within file?
   [[nodiscard]] SharedPtr<Shader> load_shader(String const &filename);

   [[nodiscard]] ShaderProgram::Id create_program( Vector<SharedPtr<Shader>> shaders );

private:
  /*
  UnorderedMap<ShaderProgramId,GLuint>  _shader_programs; // maps unique shader Ids to loaded shaders. 
  UnorderedMap<ShaderId,GLuint>         _shaders;         // maps unique shader Ids to loaded shaders. 
  */
  HashMap<String,WeakPtr<Shader>>  _loaded_shaders_map;         // maps filenames to weak pointers
};
