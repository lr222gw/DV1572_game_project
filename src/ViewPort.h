#include "misc/defs.h"
#include "Config.h"
#include "ShaderProgram.h"

// TODO: (l�g prioritet) l�gg till en  void make_active(); som i princip bara kallar p� _write_to_buffer();
// TODO: fundera �ver multi-views och hur man ska hantera det
// TODO: upp�t vektorn baserat p� normalen av ytan spelaren st�r p�
class Viewport {
public:
   Viewport(Vec3 position = { 0,0,0 }, Float32 fov = config::fov_rad): 
      _fov(fov)
   {
      // TODO: bind _camera och uniform buffer f�r Mat4
      
      Uint32 cam_loc = glGenUniformLocation();

      _model = Mat4(1.0f);
      _view = glm::lookAt(position, position + Vec3{0, 0, 1}, Vec3{ 0,1,0 });
      _projection = glm::perspective(fov,
                                    config::aspect_ratio,
                                    config::near_plane, 
                                    config::far_plane);
      
      _write_to_buffer();
    
   }

   void transform(Mat4 transformation) {
      _model *= transformation;
      _write_to_buffer();
   }

   void bind_shader_program(ShaderProgram &shapro) {
      
      _location = glGetUniformLocation(shapro.getProgramLoc(), "view_tranform");
      _write_to_buffer();
   }

private:
   void _write_to_buffer() {
      
      _camera = _projection * _view * _model;
      glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(_camera));
   }

   Float32 _fov;
   Mat4 _model, _view, _projection, _camera;
   GLuint _location;
};