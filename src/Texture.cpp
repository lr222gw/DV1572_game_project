#include "Texture.h"

Uint32 Texture::_load_texture_from_file( FilePath path ) {
   Uint32  texture_id;
   glGenTextures( 1, &texture_id );

   Int32  width,
          height,
          channel_count;

   Uint8 *image_data = stbi_load( path.relative_path().c_str(),
                                  &width,
                                  &height,
                                  &channel_count,
                                  0 );

   if ( image_data ) {
      GLenum format;
      if      ( 1 == channel_count )
         format = GL_RED;
      else if ( 3 == channel_count )
         format = GL_RGB;
      else if ( 4 == channel_count )
         format = GL_RGBA;
      else assert( false && "Unexpected texture format channel count." );

      glBindTexture( GL_TEXTURE_2D, texture_id );

      glTexImage2D( GL_TEXTURE_2D,
                    0,
                    format,
                    width,
                    height,
                    0,
                    format,
                    GL_UNSIGNED_BYTE,
                    image_data );

      glGenerateMipmap( GL_TEXTURE_2D );

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_WRAP_S,
                       GL_REPEAT );

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_WRAP_T,
                       GL_REPEAT );

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_MIN_FILTER,
                       GL_LINEAR_MIPMAP_LINEAR );

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_MAG_FILTER,
                       GL_LINEAR );
   } // TODO: use {fmt} here for formatting?
   else assert( false && String( "Error while trying to load texture from file using:"
                                 "load_texture_from_file( "+ path.relative_path() +"). ").c_str() );

   stbi_image_free( image_data );

   return texture_id;
}

Uint32 Texture::get_location() const {
   return _location;
}


Texture::Type Texture::get_type() const {
   return _type;
}

String to_string( Texture::Type t ) {
   String s;
   switch (t) {
      case Texture::Type::diffuse:   s = "diff";  break;
      case Texture::Type::normal:    s = "norm";  break;
      case Texture::Type::specular:  s = "spec";  break;
      case Texture::Type::emission:  s = "emit";  break;
      default: s = "ERROR";
   }
   return s;
}
