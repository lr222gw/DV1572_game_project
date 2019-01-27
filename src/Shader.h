#pragma once

#include "misc/defs.h"

MAKE_ENUM( ShaderType, Uint8, vertex, geometry, fragment ); // TODO: add more shaders here AND in get_type

class Shader {
public:
   Shader( char const *shader_str, ShaderType const type ) {
      // local buffer to store error strings when compiling.
      char buffer[1024];
      memset( buffer, 0, 1024 );
      GLint compile_result = 0;

      switch ( type ) {
         case ShaderType::vertex:   _shader_location = glCreateShader( GL_VERTEX_SHADER   ); break;
         case ShaderType::geometry: _shader_location = glCreateShader( GL_GEOMETRY_SHADER ); break;
         case ShaderType::fragment: _shader_location = glCreateShader( GL_FRAGMENT_SHADER ); break;
         default: throw {}; // TODO: exception?
      }

      // ask GL to use this string a shader code source
      glShaderSource( _shader_location, 1, &shader_str, nullptr );

      // try to compile this shader source.
      glCompileShader( _shader_location );

      // check for compilation error
      glGetShaderiv( _shader_location, GL_COMPILE_STATUS, &compile_result );
      if ( compile_result == GL_FALSE ) {
         // query information about the compilation (nothing if compilation went fine!)
         glGetShaderInfoLog( _shader_location, 1024, nullptr, buffer );
         // print to Visual Studio debug console output
         OutputDebugStringA( buffer );
      }
   }

   ~Shader() {
      glDeleteShader( _shader_location );
   }

   GLuint get_location() const {
      return _shader_location;
   }

private:
   ShaderType  _type;
   GLuint      _shader_location;
};
