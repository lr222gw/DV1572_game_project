#pragma once;
#include "misc/defs.h"

struct Config {
   static constexpr Int32     height = 768,
                              width  = 1024;

   static constexpr Float32   near_plane = 0.01f,
                              far_plane  = 100.0f,
                              fov_rad    = 0.01f;

   String const shader_path { "./dat/shader/" };
   String const model_path  { "./dat/models/" };
} g_config;

