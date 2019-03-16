#include "Shader.h"

Shader::Shader( char const *shader_str, Type const type ):
   _type ( type )
{
   // local buffer to store error strings when compiling.
   char buffer[1024];
   memset( buffer, 0, 1024 );
   GLint compile_result { 0 };

   switch ( type ) {
      case Type::vertex:   _shader_location = glCreateShader( GL_VERTEX_SHADER          );  break;
      case Type::geometry: _shader_location = glCreateShader( GL_GEOMETRY_SHADER        );  break;
      case Type::fragment: _shader_location = glCreateShader( GL_FRAGMENT_SHADER        );  break;
      case Type::tess_ctrl: _shader_location = glCreateShader( GL_TESS_CONTROL_SHADER   );  break;
      case Type::tess_eval: _shader_location = glCreateShader( GL_TESS_EVALUATION_SHADER);  break;
      default: assert( false && "TODO: add new ShaderType for Shader.cpp" ); // TODO: exception?
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
      //OutputDebugStringA( buffer );
      std::cerr << buffer << "\n";
   }
}

Shader::~Shader() {
   glDeleteShader( _shader_location );
}

GLuint Shader::get_location() const {
   return _shader_location;
}

Shader::Type Shader::get_type() const {
   return _type;
}
