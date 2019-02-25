#pragma once

#include "misc/defs.h"
#include "SceneManager.h"

namespace debug {
   void view_mat4( Mat4 const &, String name ); // name also serves as window title & ID
   void lightsource( Vec3 &position, Vec3 &direction, SceneManager & );
}; // namespace end

