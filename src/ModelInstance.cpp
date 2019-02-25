#include "ModelInstance.h"

void ModelInstance::draw() {
   // transfer transform (model) matrix to the shader pogram:
   glUniformMatrix4fv( _shader_program->get_transform_location(),
                       1,
                       GL_FALSE,
                       &(_transform.matrix[0][0]) );
   // index for picking
   glUniform1f( glGetUniformLocation(_shader_program->get_location(), "obj_id"),
				(float)_obj_id);

   // draw model:
   _model->draw(*_shader_program);
}

void ModelInstance::transform( Transform const &transform ) {
   _transform *= transform;
}

void ModelInstance::set_transform( Transform const &transform ) {
   _transform = transform;
}

SharedPtr<Model const> ModelInstance::get_model() const {
   return _model;
}

SharedPtr<ShaderProgram>  ModelInstance::get_shader_program() {
   return _shader_program;
}
void ModelInstance::set_shader_program(SharedPtr<ShaderProgram> shaderprogram) {
   _shader_program = shaderprogram;
}