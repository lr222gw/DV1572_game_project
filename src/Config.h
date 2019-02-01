#pragma once;
#include "misc/defs.h"

namespace config {
   static constexpr Int32     height = 768,
                              width = 1024;

   static constexpr Float32   near_plane = 0.01f,
                              far_plane = 100.0f,
                              fov_rad = 1.5708f, //90 grader i radianer
                              aspect_ratio = (Float32)height / (Float32)width;

   String const shader_path { "dat/shaders/" };
   String const model_path  { "dat/models/" };
   String const texture_path{ "dat/models/" }; //TODO: behöver vi en Texture mapp?
};

enum class FileType { model, texture };
struct FilePath {
   // konstruktor och sådant
   FilePath(FileType type, String filename) :
      _type(type),
      _filename(filename)
   {}
   //TODO: fundera över att spara som c-sträng istället... 
   String relative_path() const {
      
      String path;

      switch (_type) {         
      case (FileType::texture): path += config::texture_path; break;
      default: assert(false && "Unkown FileType");
      }
      path += _filename;
      return path;
   }

   String filename() {
      return _filename;
   }
private:
   FileType _type;
   String _filename;
};