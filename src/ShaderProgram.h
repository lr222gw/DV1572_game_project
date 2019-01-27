#pragma once
5
#include "Shader.h"
#include "misc/defs.h"

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
public:
   using Id = Uint32; // 32-bit representation used to representate shader programs with unique Ids.
   
   // TODO: (low priority) make constructor private and make make_shared a friend
   ShaderProgram(
      Vector<SharedPtr<Shader>> const &shader_ptrs
      /*, UniformInitializer initializer*/);
         //initializer list finns i .cpp filen

   ~ShaderProgram();

private:
   GLuint                     _program_location;
   Vector<SharedPtr<Shader>>  _shader_ptrs;
};