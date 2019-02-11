#pragma once
#include "Shader.h"
#include "misc/defs.h"

class ShaderProgram {
public:
   // TODO: (low priority) make constructor private and make make_shared a friend
   // initializer list finns i .cpp filen
   ShaderProgram( Vector<SharedPtr<Shader>> const &shader_ptrs );
   ShaderProgram(const ShaderProgram &shader_program);
   ~ShaderProgram();
   [[nodiscard]] GLuint get_location() const;
   [[nodiscard]] GLuint get_transform_location() const;

private:
   GLuint                     _program_location;
   GLuint                     _transform_location;
   Vector<SharedPtr<Shader>>  _shader_ptrs;
};
