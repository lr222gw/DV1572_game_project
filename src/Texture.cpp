#include "Texture.h"

String to_string( TextureType t ) {
   String s;
   switch (t) {
      case TextureType::diffuse:   s = "diff";  break;
      case TextureType::normal:    s = "norm";  break;
      case TextureType::specular:  s = "spec";  break;
      case TextureType::emissive:  s = "emit";  break;
      default: s = "ERROR";
   }
   return s;
}

// constructor binds textures at the scope of creation
TextureSet::ScopedBindGuard::ScopedBindGuard( TextureSet const &target,
                                              ShaderProgram    &sp ):
   _textures ( target )
{
   auto shader_location = sp.get_location();

   glActiveTexture( GL_TEXTURE0 );
   glUniform1i( glGetUniformLocation( shader_location, "tex_diff"), 0 );
   glBindTexture( GL_TEXTURE_2D, _textures.diffuse->get_location() );

   glActiveTexture( GL_TEXTURE1 );
   glUniform1i( glGetUniformLocation( shader_location, "tex_spec"), 1 );
   glBindTexture( GL_TEXTURE_2D, _textures.specular->get_location() );

   glActiveTexture( GL_TEXTURE2 );
   glUniform1i( glGetUniformLocation( shader_location, "tex_norm"), 2 );
   glBindTexture( GL_TEXTURE_2D, _textures.normal->get_location() );

   glActiveTexture( GL_TEXTURE3 );
   glUniform1i( glGetUniformLocation( shader_location, "tex_emit"), 3 );
   glBindTexture( GL_TEXTURE_2D, _textures.emissive->get_location() );
}

// destructor unbinds textures at the end of the scope of creation
TextureSet::ScopedBindGuard::~ScopedBindGuard() {
   for ( GLuint i = 0;  i < 4;  ++i ) {
      glActiveTexture( GL_TEXTURE0 + i );
      glBindTexture(   GL_TEXTURE_2D, 0 );
   }
}

