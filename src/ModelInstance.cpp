#include "ModelInstance.h"

void ModelInstance::draw() {
   // f�r �ver _transform till shader programmet
   glUniformMatrix4fv(_shader_program->get_transform_location(), 1, GL_FALSE, glm::value_ptr(_transform));
   _model->draw(*_shader_program);  // f�r �ver vertriser osv till gpu
}

void ModelInstance::transform(Mat4 transform) {
   _transform *= transform;
}
