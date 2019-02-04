#include "ModelInstance.h"

void ModelInstance::draw() {
   // för över _transform till shader programmet
   glUniformMatrix4fv(_shader_program->get_transform_location(), 1, GL_FALSE, &(_transform.matrix[0][0]));
   _model->draw(*_shader_program);  // för över vertriser osv till gpu
}

void ModelInstance::transform(Transform const &transform) {
   _transform *= transform;
}
