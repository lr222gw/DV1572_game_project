#include "Mesh.h"

void Mesh::_initialize_mesh() {
   glGenVertexArrays( 1, &_vao );
   glGenBuffers(      1, &_vbo );
   glGenBuffers(      1, &_ebo );

   glBindVertexArray( _vao );
   glBindBuffer( GL_ARRAY_BUFFER, _vbo );

   glBufferData( GL_ARRAY_BUFFER,
                 _vertices.size() * sizeof(VertexData),
                 &_vertices[0],
                 GL_STATIC_DRAW );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );

   glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                 _indices.size() * sizeof(GLuint),
                 &_indices[0],
                 GL_STATIC_DRAW );

   // attributes for VertexData struct: position, normal, UV

   // position
   glEnableVertexAttribArray(0);
   glVertexAttribPointer( 0, // 0 indicates that this is the first element of the struct
                          3, // no. dimensions (position = Vec3)
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(VertexData),
                          (GLvoid*)offsetof(VertexData, position) );

   // normal
   glEnableVertexAttribArray(1);
   glVertexAttribPointer( 1, // 1 indicates that this is the second element of the struct
                          3, // no. dimensions (normal = Vec3)
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(VertexData),
                          (GLvoid*)offsetof(VertexData, normal) );

   // texture
   glEnableVertexAttribArray(2);
   glVertexAttribPointer( 2, // 2 indicates that this is the third element of the struct
                          2, // no. dimensions (UV = Vec2)
                          GL_FLOAT,
                          GL_FALSE, sizeof(VertexData),
                          (GLvoid*)offsetof(VertexData, uv) );

   glBindVertexArray(0);
}

void Mesh::_draw( ShaderProgram &shader_program ) const {
   GLuint diff_num = 0;
   GLuint spec_num = 0;
   GLuint norm_num = 0;

   glUseProgram( shader_program.get_location() );

   for ( Uint32 i = 0;  i < _textures.size();  ++i ) {

      // ställa in vilken Textur vi jobbar på, vi kan högst ha 32 (eller 16?)  TODO
      glActiveTexture( GL_TEXTURE0 + i );
      // varv 1; i = 0; GL_TEXTURE0 + i = GL_TEXTURE0;;; varv 2; i = 1 ; GL_TEXTURE0 + i = GL_TEXTURE1, osv. TODO

      // fetch the data
      StringStream  stream;

      auto type = _textures[i]->get_type();


      stream << "tex_"; // output prefix, then type and type number:
      switch ( type ) {
         case Texture::Type::diffuse:   stream << to_string(type) << ++diff_num;  break;
         case Texture::Type::specular:  stream << to_string(type) << ++spec_num;  break;
         case Texture::Type::normal:    stream << to_string(type) << ++norm_num;  break;
      // case Texture::Type::emission:  stream << to_string(type) << ++emit_num;  break;
         default: assert( false && "Unknown texture type!" );
      }

      String texture_channel = stream.str();

      glUniform1i( glGetUniformLocation( shader_program.get_location(),  texture_channel.c_str() ),
                   i );

      glBindTexture( GL_TEXTURE_2D, _textures[i]->get_location() );
   }

   // glUniform1f( glGetUniformLocation(shader_program.get_location(), "material.shininess" ), 16.0f); TODO
   glBindVertexArray( _vao );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );

   glDrawElements( GL_TRIANGLES,
                   _indices.size(),
                   GL_UNSIGNED_INT,
                   0);

   // glDrawArrays( GL_TRIANGLES, this->vertex_list[0].position.x, GL_UNSIGNED_INT ); TODO

   // bind OpenGL to standard values
   glBindVertexArray(0);

   for ( GLuint i = 0;  i < _textures.size();  ++i ) {
      glActiveTexture( GL_TEXTURE0 + i );
      glBindTexture( GL_TEXTURE_2D, 0 );
   }
}
