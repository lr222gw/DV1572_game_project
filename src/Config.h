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

   static constexpr Uint32    particle_max_count = 8192;

   static constexpr Float32   near_plane         =  0.001f,
                              far_plane          =  500.0f,
                              fov_rad            =  1.2708f, // 90 degrees in radians
                              start_aspect_ratio = (Float32)start_height / (Float32)start_width;

   static constexpr Bool      is_debugging = true;

   static String const  shader_path;
   static String const   model_path;
   static String const texture_path; //TODO: unneeded?

/*------------------------------ global variables -------------------------------*/
   Bool is_wireframe_mode = false; // used in SceneManager::Draw()
   Bool is_imgui_toggled  = false;
   RenderMode render_mode = RenderMode::composite;


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
