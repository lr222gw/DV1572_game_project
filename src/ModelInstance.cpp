#include "ModelInstance.h"

ModelInstance::ModelInstance( SharedPtr<Model>   model,
               SharedPtr<ShaderProgram>          shader_program,
               Transform const                  &transform,
               std::function<void()>             callback_on_transform,
               Bool                              is_tessellation_enabled,
               Uint32                            id )
:
   _model                   ( model                   ),
   _shader_program          ( shader_program          ),
   _transform               ( transform               ),
   _callback_on_transform   ( callback_on_transform   ),
   _is_tessellation_enabled ( is_tessellation_enabled ),
   id                       ( id                      ),
   model_transform          ( _transform              )
{
   // ShadowMap needs to be updated if a new model is loaded..
   _callback_on_transform();
}

void ModelInstance::draw() {
   // transfer transform (model) matrix to the shader pogram:
   //if (!_is_tessellation_enabled) {
      _shader_program->use();
   //}
   

   glUniformMatrix4fv( _shader_program->get_transform_location(),
                       1,
                       GL_FALSE,
                       &(_transform.matrix[0][0]) );

   // convert ID to RGBA color:
   Vec4 id_as_rgba { (Float32((id) >>  0 & 0xFF)) / 255,
                     (Float32((id) >>  8 & 0xFF)) / 255,
                     (Float32((id) >> 16 & 0xFF)) / 255,
                     (Float32((id) >> 24 & 0xFF)) / 255 };

   // index for picking
   glUniform4fv( glGetUniformLocation(_shader_program->get_location(), "obj_id"),
                 1,
                 glm::value_ptr(id_as_rgba));

   // draw model:
   _model->_draw( *_shader_program, _is_tessellation_enabled );
}

void ModelInstance::transform( Transform const &transform ) {
   _transform *= transform;
   _callback_on_transform();
} // TODO: move semantics

void ModelInstance::set_transform( Transform const &transform ) {
   _transform = transform;
   _callback_on_transform();
} // TODO: move semantics

Transform ModelInstance::get_transform() const {
   return _transform;
}

SharedPtr<ShaderProgram>  ModelInstance::get_shader_program() {
   return _shader_program;
} // TODO: move semantics

void ModelInstance::set_shader_program( SharedPtr<ShaderProgram> shader_program ) {
   _shader_program = shader_program;
} // TODO: move semantics

SharedPtr<Model const> ModelInstance::get_model() const {
   return _model;
}

void ModelInstance::set_model( SharedPtr<Model> model ) {
   _model = model;
}

Bool ModelInstance::get_is_tessellation_enabled() const {
   return _is_tessellation_enabled;
}

void ModelInstance::set_is_tessellation_enabled( Bool state ) {
   _is_tessellation_enabled = state;
}






