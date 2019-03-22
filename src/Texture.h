#pragma once

#include "misc/defs.h"
#include "Config.h"
#include "ShaderProgram.h"

enum class TextureType : Uint8 { diffuse      = 0,
                                 normal       = 1,
                                 specular     = 2,
                                 emissive     = 3,
                                 displacement = 4 };

template <TextureType T_type>
class Texture {
public:
   static constexpr auto type = T_type; // expose type

   Texture():
      _location ( _generate_default_texture() )
   {}

   Texture( FilePath path ):
      _location ( _load_texture_from_file(path) )
   {} // TODO: handle errors?

   Texture( Texture const & ) = delete;

   Texture( Texture &&other ):
      _location ( other._location )
   {}

   ~Texture() {
      glDeleteTextures( 1, &_location );
   }

   Texture& operator=( Texture const &  ) = delete;
   Texture& operator=( Texture       && ) = delete;

   [[nodiscard]] Uint32   get_location() const {
      return _location;
   }

private:
   Uint32    _location;

   Uint32 _load_texture_from_file( FilePath path ) {
      Uint32  texture_id;
      glGenTextures( 1, &texture_id );

      Int32  width,
             height,
             channel_count;

      // freed at end of function
      Uint8 *image_data = stbi_load( path.relative_path().c_str(),
                                     &width,
                                     &height,
                                     &channel_count,
                                     0 );

      if ( image_data ) {
         GLenum format;
         if      ( 1 == channel_count ) {
            format = GL_RED;
            if constexpr ( Config::is_debugging ) printf( "Loading GL_RED texture '%s'\n", path.relative_path().c_str() );
         }
         else if ( 3 == channel_count ) {
            format = GL_RGB;
            if constexpr ( Config::is_debugging ) printf( "Loading GL_RGB texture '%s'\n", path.relative_path().c_str() );
         }
         else if ( 4 == channel_count ) {
            format = GL_RGBA;
            if constexpr ( Config::is_debugging ) printf( "Loading GL_RGBA texture '%s'\n", path.relative_path().c_str() );
         }
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
                          GL_LINEAR );

         glTexParameteri( GL_TEXTURE_2D,
                          GL_TEXTURE_MAG_FILTER,
                          GL_LINEAR );

      } // TODO: use {fmt} here for formatting?
      else assert( false && String( "Error while trying to load texture from file using:"
                                    "load_texture_from_file( "+ path.relative_path() +"). ").c_str() );

      stbi_image_free( image_data );

      return texture_id;
   }

   Uint32 _generate_default_texture() {
      Uint8   pixel_data[4];
      GLenum  format;
      Uint32  texture_id;

      glGenTextures( 1, &texture_id );

      if constexpr ( type == TextureType::diffuse ) {
         pixel_data[0] = 128;
         pixel_data[1] = 128;
         pixel_data[2] = 128;
         pixel_data[3] = 255;
         format        = GL_RGBA;
      }
      else if      ( type == TextureType::normal ) {
         pixel_data[0] = 128;
         pixel_data[1] = 128;
         pixel_data[2] = 255;
         format        = GL_RGB;
      }
      else if      ( type == TextureType::specular ) {
         pixel_data[0] = 128;
         pixel_data[1] = 128;
         pixel_data[2] = 128; // TODO: contemplate proper default value
         pixel_data[3] = 128;
         format        = GL_RGBA;
      }
      else if      ( type == TextureType::emissive ) {
         pixel_data[0] = 0;
         pixel_data[1] = 0;
         pixel_data[2] = 0;
         format        = GL_RGBA; // TODO: change into GL_RGB?
      }
      else if      ( type == TextureType::displacement ) {
         pixel_data[0] = 128;
         pixel_data[1] = 128;
         pixel_data[2] = 128;
         format        = GL_RGB; // TODO: use single channel
      }
      else assert( false && "Unaccounted for texture type!" );

      glBindTexture( GL_TEXTURE_2D, texture_id );

      glTexImage2D( GL_TEXTURE_2D,
                    0,
                    format,
                    1, // height
                    1, // width
                    0,
                    format,
                    GL_UNSIGNED_BYTE,
                    pixel_data );

      glGenerateMipmap( GL_TEXTURE_2D );

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_WRAP_S,
                       GL_REPEAT );

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_WRAP_T,
                       GL_REPEAT );

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_MIN_FILTER,
                       GL_NEAREST );

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_MAG_FILTER,
                       GL_NEAREST );

      return texture_id;
   }
};

[[nodiscard]] String to_string( TextureType );

using NormalTexture       = Texture<TextureType::normal>;
using SpecularTexture     = Texture<TextureType::specular>;
using DiffuseTexture      = Texture<TextureType::diffuse>;
using EmissiveTexture     = Texture<TextureType::emissive>;
using DisplacementTexture = Texture<TextureType::displacement>;

class TextureSet {
public:
   /* RAII wrapper for scoped bindings. */
   class ScopedBindGuard {
   public:
      // binds textures at scope after construction
      ScopedBindGuard( TextureSet      const &, ShaderProgram & );
      ScopedBindGuard( ScopedBindGuard const &  ) = delete;
      ScopedBindGuard( ScopedBindGuard       && ) = delete;
      auto operator=(  ScopedBindGuard const &  ) = delete;
      auto operator=(  ScopedBindGuard       && ) = delete;
     ~ScopedBindGuard(); // unbinds textures at end of scope
   private:
      TextureSet const &_textures;
   };

// exposed member data:
   SharedPtr<DiffuseTexture>       diffuse;      // 0
   SharedPtr<NormalTexture>        normal;       // 1
   SharedPtr<SpecularTexture>      specular;     // 2
   SharedPtr<EmissiveTexture>      emissive;     // 3
   SharedPtr<DisplacementTexture>  displacement; // 4
   static constexpr Uint32         channel_count  = 5; // remember to update when adding channels
};

