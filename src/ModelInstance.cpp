#include "ModelInstance.h"

void ModelInstance::draw() {
   // transfer transform (model) matrix to the shader pogram:
   glUniformMatrix4fv( _shader_program->get_transform_location(),
                       1,
                       GL_FALSE,
                       &(_transform.matrix[0][0]) );

   Uvec4 id_as_rgba { id >>  0 & 0xFF,
                      id >>  8 & 0xFF,
                      id >> 16 & 0xFF,
                      id >> 24 & 0xFF };

   // index for picking
   glUniform4fv( glGetUniformLocation(_shader_program->get_location(), "obj_id"),
					1,
					glm::value_ptr(id_as_rgba));

   // draw model:
   _model->draw(*_shader_program);
}

void ModelInstance::transform( Transform const &transform ) {
   _transform *= transform;
   _callback_on_transform();
}

void ModelInstance::set_transform( Transform const &transform ) {
   _transform = transform;
   _callback_on_transform();
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

Uint32 ModelInstance::_generate_id() const {
	static Uint32 next_id = 0;
	return next_id++;
}
