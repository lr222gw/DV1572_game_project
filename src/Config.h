#pragma once

#include "misc/defs.h"

enum RenderMode : Uint32 {       composite  = 0,
                                 albedo     = 1,
                                 normals    = 2,
                                 specular   = 3,
                                 positional = 4  }; // TODO: emission!

enum class FileType   { model, texture };

struct Config {
   static constexpr Int32     start_height =  768,
                              start_width  = 1024;

   static constexpr Float32   near_plane         =  0.01f,
                              far_plane          =  200.0f,
                              fov_rad            =  1.5708f, // 90 degrees in radians
                              start_aspect_ratio = (Float32)start_height / (Float32)start_width;

   static char constexpr  shader_path[] = { "dat/shaders/" };
   static char constexpr   model_path[] = { "dat/models/"  };
   static char constexpr texture_path[] = { "dat/models/"  }; //TODO: unneeded?

/*------------------------------ global variables -------------------------------*/
   Bool is_wireframe_mode = false; // used in SceneManager::Draw()
   RenderMode render_mode = RenderMode::composite;


} extern config; // <- our global Config instance declaration, defined in Config.cpp

struct FilePath {
   // konstruktor och s�dant
   FilePath( FileType type, String filename ):
      _type     ( type     ),
      _filename ( filename )
   {}
   // TODO: fundera �ver att spara som c-str�ng ist�llet... 
   String relative_path() const {    
      String path;
      switch (_type) {         
         case FileType::texture: path += Config::texture_path; break;
         default: assert( false && "Unknown file type." );
      }
      path += _filename;
      return path;
   }

   String filename() {
      return _filename;
   }
private:
   // TODO: tv� pekare till samma Cstring
   FileType _type;
   String   _filename;
};
