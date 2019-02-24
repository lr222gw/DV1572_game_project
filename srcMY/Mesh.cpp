#include "Mesh.h"

void Mesh::_initialize_mesh() {
   glGenVertexArrays( 1, &_vao );
   glGenBuffers(      1, &_vbo );
   glGenBuffers(      1, &_ebo );
   
   glBindVertexArray( _vao );
   glBindBuffer( GL_ARRAY_BUFFER, _vbo );

   glBufferData( GL_ARRAY_BUFFER,
                 _vertex_list.size() * sizeof(VertexData),
                 &_vertex_list[0],
                 GL_STATIC_DRAW );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );
   
   glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                 _index_list.size() * sizeof(GLuint),
                 &_index_list[0],
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

void Mesh::_draw( ShaderProgram &shader_program ) {
   GLuint diff_num = 0;
   GLuint spec_num = 0;
   GLuint norm_num = 0;

   glUseProgram( shader_program.get_location() );

   for ( Uint32 i = 0;  i < _texture_list.size();  ++i ) {
      
      // ställa in vilken Textur vi jobbar på, vi kan högst ha 32 (eller 16?)  TODO
      glActiveTexture( GL_TEXTURE0 + i ); 
      // varv 1; i = 0; GL_TEXTURE0 + i = GL_TEXTURE0;;; varv 2; i = 1 ; GL_TEXTURE0 + i = GL_TEXTURE1, osv. TODO

      // fetch the data
      StringStream  stream; 
      String        number;
      String        name    = _texture_list[i].type; 

      if      ( "tex_diff" == name )
         stream  <<  ++diff_num;
      else if ( "tex_spec" == name )
         stream  <<  ++spec_num;
      else if ( "tex_norm" == name )
         stream  <<  ++norm_num;

      number = stream.str();
      
      glUniform1i(glGetUniformLocation( shader_program.get_location(),
                                        (name + number).c_str()),
                                        i );
      glBindTexture( GL_TEXTURE_2D,
                     _texture_list[i].id );
   }

   // glUniform1f( glGetUniformLocation(shader_program.get_location(), "material.shininess" ), 16.0f); TODO
   glBindVertexArray( _vao );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );

   glDrawElements( GL_TRIANGLES,
                   _index_list.size(),
                   GL_UNSIGNED_INT,
                   0);

   // glDrawArrays( GL_TRIANGLES, this->vertex_list[0].position.x, GL_UNSIGNED_INT ); TODO
   
   // bind OpenGL to standard values
   glBindVertexArray(0);

   for ( GLuint i = 0;  i < _texture_list.size();  ++i ) {
      glActiveTexture( GL_TEXTURE0 + i );
      glBindTexture( GL_TEXTURE_2D, 0 );
   }
}
