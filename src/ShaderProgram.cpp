#include "ShaderProgram.h"

ShaderProgram::ShaderProgram( Vector<SharedPtr<Shader>> const &shader_ptrs):
   _shader_ptrs(shader_ptrs)
{
   // local buffer to store error strings when compiling.
   char buffer[1024];
   memset(buffer, 0, 1024);

   //initializer();

   _program_location = glCreateProgram();

   for (auto &shader_ptr : shader_ptrs)
      glAttachShader(_program_location, shader_ptr->get_location());

   glLinkProgram(_program_location);

   auto compile_result = GL_FALSE;
   glGetProgramiv(_program_location, GL_LINK_STATUS, &compile_result);
   if (compile_result == GL_FALSE) {
      // query information about the compilation (nothing if compilation went fine!)
      memset(buffer, 0, 1024);
      glGetProgramInfoLog(_program_location, 1024, nullptr, buffer);
      // print to Visual Studio debug console output
      std::cerr << buffer << "\n"; // TODO: ersätt med assert(...)
      // OutputDebugStringA(buffer);
   }

   // förvarar en modellinstans transform
   _location = glGetUniformLocation( _program_location, "model_transform" );

  // auto id = _generate_shader_program_id();
  // _shader_programs[id] = _program_location;
}

GLuint ShaderProgram::getProgramLoc() const {
   return _program_location;
}

GLuint ShaderProgram::get_transform_location() const {
   return _transform_location; // används i ModelInstance::draw() för att föra över instansens
   // lokals transform till shader programmen så att modellen kan transformeras till korrekt
   // globala position samt rotation osv. i slutändan.
}

ShaderProgram::~ShaderProgram() {
   for (auto &shader_ptr : _shader_ptrs)
      glDetachShader(_program_location, shader_ptr->get_location());

   glDeleteProgram(_program_location);
}
