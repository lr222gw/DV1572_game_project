#include "Texture.h"

String to_string( TextureType t ) {
   String s;
   switch (t) {
      case TextureType::diffuse:   s = "diff";  break;
      case TextureType::normal:    s = "norm";  break;
      case TextureType::specular:  s = "spec";  break;
      case TextureType::emission:  s = "emit";  break;
      default: s = "ERROR";
   }
   return s;
}
