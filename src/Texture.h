#pragma once

#include "misc/defs.h"
#include "Config.h"


class Texture {
public:
   enum class Type : Uint8 { diffuse  = 0,
                             normal   = 1,
                             specular = 2,
                             emission = 3 };

   Texture( FilePath path, Type type ):
      _path     ( path ),
      _type     ( type ),
      _location ( _load_texture_from_file(path) )
   {} // TODO: handle errors?

   Texture( Texture const & ) = delete;

   Texture( Texture &&other ):
      _path     ( other._path     ), // TODO: std::move if FilePath ever implements move constructor
      _type     ( other._type     ),
      _location ( other._location )
   {}

   ~Texture() {
      glDeleteTextures( 1, &_location );
   }

   Texture& operator=( Texture const &  ) = delete;
   Texture& operator=( Texture       && ) = delete;

   [[nodiscard]] Uint32 get_location() const;
   [[nodiscard]] Type   get_type()     const;

private:
   FilePath  _path;
   Type      _type;
   Uint32    _location;

   Uint32    _load_texture_from_file( FilePath );
};

[[nodiscard]] String to_string( Texture::Type );
