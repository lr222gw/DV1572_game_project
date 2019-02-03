#pragma once

#include "misc/defs.h"

//MAKE_ENUM(ShaderType, Uint8, vertex, geometry, fragment); // TODO: add more shaders here AND in get_type

class Shader {
public:
   enum class Type { vertex, geometry, fragment, error };

   using Id = Uint32; // 32-bit representation used to represent shader with uniqie Ids.
   Shader( char const *shader_str, Type const type);

   ~Shader();

   GLuint  get_location() const;
   Type    get_type()     const;

private:
   Type        _type;
   GLuint      _shader_location;
};
