#pragma once

#include "misc/defs.h"

namespace debug {
   void view_mat4( Mat4 const &, String name ); // name also serves as window title & ID
   void lightsource( Vec3 &position, Vec3 &direction );
}; // namespace end

