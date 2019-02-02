#include "misc/defs.h"
#include "Config.h"
#include "ShaderProgram.h"

// TODO: (låg prioritet) lägg till en  void make_active(); som i princip bara kallar på _write_to_buffer();
// TODO: fundera över multi-views och hur man ska hantera det
// TODO: uppåt vektorn baserat på normalen av ytan spelaren står på
class Viewport {
public:
   Viewport(Vec3 position = { 0,0,0 }, Vec3 rotation, Float32 fov = config::fov_rad);

   void transform(Mat4 transformation);

   void bind_shader_program(ShaderProgram &shapro);

private:
   void _write_to_buffer();

   Float32 _fov;
   Mat4 _model, _view, _projection, _camera;
   GLuint _location;
};