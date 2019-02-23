#pragma once

#include "misc/defs.h"
//#include "Light.h"
class Light; // Forward Declaration; we only point to Light class, does not store instances of it or use Light function

class Shadowcaster {
public:
   Shadowcaster(SharedPtr<Light> source) : _source(source) {};
   Shadowcaster(           SharedPtr<Light> source, 
                           Float32 n_plane, 
                           Float32 f_plane, 
                           Float32 left, 
                           Float32 right, 
                           Float32 bottom, 
                           Float32 top, 
                           Vec3 pos, 
                           Vec3 target,   
                           Vec3 up = Vec3(0.0f,1.0f,0.0f));

   void set_Light_matrix(  Float32 n_plane,
                           Float32 f_plane,
                           Float32 left,
                           Float32 right,
                           Float32 bottom,
                           Float32 top,
                           Vec3 pos,
                           Vec3 target,
                           Vec3 up = Vec3(0.0f, 1.0f, 0.0f));
   Mat4 get_matrix();
private:

   SharedPtr<Light>  _source;
   Mat4              _light_matrix;
   Mat4              _light_view;
   Mat4              _light_proj;

   Float32           _n_plane, _f_plane, _left, _right, _bottom, _top;
   Vec3              _pos, _target, _up;



};