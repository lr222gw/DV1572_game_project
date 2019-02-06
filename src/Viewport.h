#include "misc/defs.h"
#include "Config.h"
#include "ShaderProgram.h"

#include "Transform.h"

struct gBufferIds {
   bool     is_set = false;
   Uint32   g_buffer;
   Uint32   g_pos_texture;
   Uint32   g_norm_texture;
   Uint32   g_spec_texture;
   Uint32   g_albedo_rgba_texture;
};

// TODO: (l�g prioritet) l�gg till en  void make_active(); som i princip bara kallar p� _write_to_buffer();
// TODO: fundera �ver multi-views och hur man ska hantera det
// TODO: upp�t vektorn baserat p� normalen av ytan spelaren st�r p�
class Viewport {
public:
   Vec3 front;
   
   Viewport(Vec3 position, GLFWwindow *window, Float32 fov = config::fov_rad);
   Viewport( Viewport  const &) = delete;
   Viewport( Viewport && )      = delete;
   Viewport& operator=( Viewport const & ) = delete;
   Viewport& operator=( Viewport && )      = delete;



   void transform( Transform const &transform );
   void set_view( Transform const &transform );
   Transform get_view() const;

   void bind_shader_program(ShaderProgram &shapro);

   // void set_position(Vec3 new_position);
   // void set_rotation(Vec3 new_rotation);

   void set_fov(Float32 fov_rad);

   void update();

private:
   // void _update_view_matrix();
   void _write_to_buffer();
   void _update_aspect_ratio();
   void _generate_perspective();
   void _g_buffer_init(float width, float height);

   // Vec3 _position,
   //      _rotation;

   gBufferIds _gbuffer_ids;

   Float32  _fov;
   Float32  _aspect;

   Transform  _view;
   Mat4       _projection;

   GLFWwindow *_window;
   // TODO: (l�g prioritet) anv�nd transform f�r projection?

   GLuint _location;
};