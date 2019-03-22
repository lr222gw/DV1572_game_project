#pragma once

#include "misc/defs.h"

enum RenderMode : Uint32 { composite    = 0,
                           albedo       = 1,
                           specular     = 2,
                           normals      = 3,
                           emissive     = 4,
                           displacement = 5,
                           positional   = 6,
                           textureless  = 7,
                           picking	    = 8 };

enum class FileType { model, texture };

struct Config {
   static constexpr Int32     start_height =  768,
                              start_width  = 1024;

   static constexpr Int32     shadowmap_side = 2048;

   static constexpr Uint32    particle_max_count = 2048;
   static constexpr Uint32    ssao_kernel_count  =   64; // needs to be equal to ssao.frag::kernel_size!
   static constexpr Uint32    ssao_noise_side    =    8; // needs to be equal to ssao.frag::noise_side & ssao_blur.frag::noise_side!
   static constexpr Float32   ssao_scale_growth_factor  = 1 / ssao_kernel_count;

   static constexpr Float32   near_plane         =  0.001f,
                              far_plane          =  500.0f,
                              fov_rad            =  1.2708f, // 90 degrees in radians
                              start_aspect_ratio = (Float32)start_height / (Float32)start_width;

#ifdef DEBUG
   static Bool constexpr  is_debugging = true;
   static Bool            should_draw_debug_elements; // toggleable
#else
   static Bool constexpr  is_debugging = false;
#endif

   static String const  shader_path;
   static String const   model_path;
   static String const texture_path; //TODO: unneeded?

/*------------------------------ global variables -------------------------------*/
   Bool        is_wireframe_mode     = false; // used in SceneManager::Draw()
   Bool        is_imgui_toggled      = false;
   Bool        is_mouse_look_enabled = false;
   Float32     fly_move_speed        = 25.0f; // TODO: revamp!
   RenderMode  render_mode           = RenderMode::composite;


} extern config; // <- our global Config instance declaration, defined in Config.cpp

// TODO: split into source
struct FilePath {
   // konstruktor och s�dant
   FilePath( FileType type, String filename ):
      _type     ( type     ),
      _filename ( filename )
   {}

   [[nodiscard]] String relative_path() const {
      String path;
      switch (_type) {
         case FileType::texture: path += Config::texture_path; break;
         // TODO: model case
         default: assert( false && "Unknown file type." );
      }
      path += _filename;
      return path;
   }

   [[nodiscard]] String filename() {
      return _filename;
   }
private:
   // TODO: (low prio) use a single C-string with two pointers instead
   FileType _type;
   String   _filename;
};
