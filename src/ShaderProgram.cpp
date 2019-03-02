#include "ShaderProgram.h"

ShaderProgram::ShaderProgram( Vector<SharedPtr<Shader>> const &shader_ptrs ):
   _shader_ptrs ( shader_ptrs )
{
   // local buffer to store error strings during compilation
   char buffer[1024];
   memset( buffer, 0, 1024 );

   _program_location = glCreateProgram();

   for (auto &shader_ptr : shader_ptrs)
      glAttachShader( _program_location, shader_ptr->get_location() );

   glLinkProgram( _program_location );


   auto compile_result = GL_FALSE;
   glGetProgramiv( _program_location, GL_LINK_STATUS, &compile_result );

   if (compile_result == GL_FALSE) {
      // query information about the compilation (nothing if compilation went fine!)
      memset( buffer, 0, 1024 );
      glGetProgramInfoLog( _program_location, 1024, nullptr, buffer );
      std::cout << buffer;
      assert( false && "[ERROR]: Shader program compilation failed." );
   }

   // förvarar en modellinstans transform
   _transform_location = glGetUniformLocation( _program_location, "model_transform" );
}

void ShaderProgram::use() {
   glUseProgram( _program_location );
}

ShaderProgram::ShaderProgram(const ShaderProgram & shader_program)
{
   _program_location   = shader_program._program_location;
   _shader_ptrs        = shader_program._shader_ptrs;
   _transform_location = shader_program._transform_location;
}

GLuint ShaderProgram::get_location() const {
   return _program_location;
}

GLuint ShaderProgram::get_transform_location() const {
   return _transform_location; // used in ModelInstance::draw() to transfer the model
   // instance's "local space"-to"global space"-transform to the shader program
}

ShaderProgram::~ShaderProgram() {
   for ( auto &shader_ptr : _shader_ptrs )
      glDetachShader( _program_location, shader_ptr->get_location() );

   glDeleteProgram( _program_location );
}
