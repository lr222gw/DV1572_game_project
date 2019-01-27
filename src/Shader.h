#pragma once

#include "misc/defs.h"

MAKE_ENUM( ShaderType, Uint8, vertex, geometry, fragment ); // TODO: add more shaders here AND in get_type

class Shader {
public:
   using Id = Uint32; // 32-bit representation used to represent shader with uniqie Ids.
   Shader( char const *shader_str, ShaderType const );

   ~Shader();

   GLuint get_location() const;

private:
   ShaderType  _type;
   GLuint      _shader_location;
};
