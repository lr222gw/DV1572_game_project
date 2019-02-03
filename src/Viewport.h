#include "misc/defs.h"
#include "Config.h"
#include "ShaderProgram.h"

// TODO: (l�g prioritet) l�gg till en  void make_active(); som i princip bara kallar p� _write_to_buffer();
// TODO: fundera �ver multi-views och hur man ska hantera det
// TODO: upp�t vektorn baserat p� normalen av ytan spelaren st�r p�
class Viewport {
public:
   Viewport(Vec3 position, Vec3 rotation, Float32 fov = config::fov_rad);

   void transform(Mat4 transformation);

   void bind_shader_program(ShaderProgram &shapro);

   void set_position(Vec3 new_position);

   void set_rotation(Vec3 new_rotation);

   void set_fov(Float32 fov_rad);


private:
   void _update_view_matrix();
   void _write_to_buffer();

   Vec3 _position,
        _rotation;

   Float32 _fov;
   Mat4 _view, _projection, _camera;
   GLuint _location;
};