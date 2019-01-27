#pragma once

#include "misc/defs.h"

class ShaderHandler {
public:
   using ShaderId        = Uint32; // 32-bit representation used to representate shaders with unique Ids.
   using ShaderProgramId = Uint32; // 32-bit representation used to representate shader programs with unique Ids.

// TODO: embed type into filename? extract from within file?
[[nodiscard]] SharedPtr<Shader> load_shader( StringView filename );

[[nodiscard]] ShaderProgramId create_program( Vector<ShaderId> shader_ids );

private:
  /*
  UnorderedMap<ShaderProgramId,GLuint>  _shader_programs; // maps unique shader Ids to loaded shaders. 
  UnorderedMap<ShaderId,GLuint>         _shaders;         // maps unique shader Ids to loaded shaders. 
  */
  UnorderedMap<String,WeakPtr<Shader>>  _loaded_shaders_map;         // maps filenames to weak pointers
};
