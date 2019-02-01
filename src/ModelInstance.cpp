#include "ModelInstance.h"

void ModelInstance::draw() {
   // för över _transform till shader programmet
   glUniformMatrix4fv(_shader_program->get_transform_location(), 1, GL_FALSE, glm::value_ptr(_transform));
   _model->draw(*_shader_program);  // för över vertriser osv till gpu
}

void ModelInstance::transform(Mat4 transform) {
   _transform *= transform;
}
