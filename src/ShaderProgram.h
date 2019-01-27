#pragma once
// 1. vi behöver iterera genom alla uniforms som nämns i shadern (t.ex. enligt stackoverflown)
// 2. men vi behöver populera uniform_data mappen och binda dess element till motsvarande openGL locations
// 
// sp.uniform_data["rotation_speed"] = Float32   { 0.4f };
// sp.uniform_data["view_matrix"]    = glm::mat4 { 1.0f };
// 
// Map<String,Variant< Int32,
//                     Float32,
//                     glm::vec2,
//                     glm::vec3,
//                     glm::vec4,
//                     glm::mat2,
//                     glm::mat3,
//                     glm::mat4 >> uniform_data;
// 
// 
// TODO: how to handle the encapsulation of uniform parameters to the shader program
class ShaderProgram {
public:5
   ShaderProgram(
      Vector<SharedPtr<Shader>> const &shader_ptrs,
      UniformInitializer initializer
   ):
      _shader_ptrs ( shader_ptrs )
   {
      // local buffer to store error strings when compiling.
      char buffer[1024];
      memset( buffer, 0, 1024 );

      initializer();

      _program_location = GlCreateProgram();

      for ( auto &shader_ptr : shader_ptrs )
         glAttachShader( _program_location, shader_ptr->get_location() );

      glLinkProgram( _program_location );

      auto compile_result = GL_FALSE;
      glGetProgramiv( _program_location, GL_LINK_STATUS, &compile_result );
      if ( compile_result == GL_FALSE ) {
         // query information about the compilation (nothing if compilation went fine!)
         memset( buffer, 0, 1024 );
         glGetProgramInfoLog( _program_location, 1024, nullptr, buffer );
         // print to Visual Studio debug console output
         OutputDebugStringA( buffer );
      }

      auto id = _generate_shader_program_id();
      _shader_programs[id] = _program_location;
   }

   ~ShaderProgram() {
      for ( auto &shader : shader_ptrs )
         glDetachShader( _program_location, shader_ptr->get_location() );
      glDeleteProgram( _program_location );
   }

private:
   GLuint                     _program_location;
   Vector<SharedPtr<Shader>>  _shader_ptrs;
};