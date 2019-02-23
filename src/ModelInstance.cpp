#include "ModelInstance.h"

void ModelInstance::draw() {
   // transfer transform (model) matrix to the shader pogram:
   glUniformMatrix4fv( _shader_program->get_transform_location(),
                       1,
                       GL_FALSE,
                       &(_transform.matrix[0][0]) );
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
