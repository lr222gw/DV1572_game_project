#include "Shadowcaster.h"

Shadowcaster::Shadowcaster(SharedPtr<Light> source, Float32 n_plane, Float32 f_plane, Float32 left, Float32 right, Float32 bottom, Float32 top, Vec3 pos, Vec3 target, Vec3 up)
{
   _source = source;
   set_Light_matrix(n_plane, f_plane, left, right, bottom, top, pos, target, up);
}

void Shadowcaster::set_Light_matrix(Float32 n_plane, Float32 f_plane, Float32 left, Float32 right, Float32 bottom, Float32 top, Vec3 pos, Vec3 target, Vec3 up)
{
   _n_plane = n_plane;
   _f_plane = f_plane;
   _left    = left;
   _right   = right;
   _bottom  = bottom;
   _top     = top;
   _pos     = pos;
   _target  = target;
   _up      = up;

   _light_view = glm::lookAt(_pos, _target, _up);
   _light_proj = glm::ortho(_left, _right, _bottom, _top, _n_plane, _f_plane);

   //_source->set_data(LightData);
   _source->set_direction(_target);
   _source->set_position(_pos);
   //_source.
   _light_matrix = _light_proj * _light_view;
}

Mat4 Shadowcaster::get_matrix()
{
   return _light_matrix;
}

Array<Float32,4> Shadowcaster::getCorners()
{
   return { _left, _right, _bottom, _top };
}
