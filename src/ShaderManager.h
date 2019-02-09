#pragma once

#include "misc/defs.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Config.h"

class ShaderManager {
public:
   // TODO: embed type into filename? extract from within file?
   [[nodiscard]] SharedPtr<Shader> load_shader( String const &filename );
      //TODO: (LOW PRIORITY) just nu kan vi ha t.ex. flera VertexShader till ett program => ogitligt program

   [[nodiscard]] SharedPtr<ShaderProgram> create_program( Vector<SharedPtr<Shader>> shaders );
      //TODO: (LOW PRIORITY) Fixa så att man inte kan göra identiska ShaderProgram! 

private:
   [[nodiscard]] Shader::Type _extract_type( StringView filename ) const;
// UnorderedMap<ShaderProgramId,GLuint>  _shader_programs; // maps unique shader Ids to loaded shaders. 
// UnorderedMap<ShaderId,GLuint>         _shaders;         // maps unique shader Ids to loaded shaders. 
   HashMap<String,WeakPtr<Shader>>  _loaded_shaders;       // maps filenames to weak pointers
   HashMap<String,Bool>             _shader_is_loaded;     // maps whether a shader has been loaded or not (TODO: unset if unloaded!)
};
