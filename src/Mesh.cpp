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

   // attributes for VertexData struct: position, normal, tangent, bitangent, UV

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

   // tangent
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, // 1 indicates that this is the second element of the struct
						 3, // no. dimensions (tangent = Vec3)
						 GL_FLOAT,
						 GL_FALSE,
						 sizeof(VertexData),
						 (GLvoid*)offsetof(VertexData, tangent));

   // bitangent
   glEnableVertexAttribArray(3);
   glVertexAttribPointer(3, // 1 indicates that this is the second element of the struct
						 3, // no. dimensions (bitangent = Vec3)
						 GL_FLOAT,
						 GL_FALSE,
						 sizeof(VertexData),
						 (GLvoid*)offsetof(VertexData, bitangent));

   // texture
   glEnableVertexAttribArray(4);
   glVertexAttribPointer( 4, // 2 indicates that this is the third element of the struct
                          2, // no. dimensions (UV = Vec2)
                          GL_FLOAT,
                          GL_FALSE, sizeof(VertexData),
                          (GLvoid*)offsetof(VertexData, uv) );

   // bind OpenGL to standard values
   glBindVertexArray(0);
}

void Mesh::_draw( ShaderProgram &shader_program ) const {
   shader_program.use();

   TextureSet::ScopedBindGuard pin { _textures, shader_program }; // RAII

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
}


void Mesh::_draw_tessellated( ShaderProgram &shader_program_tessellated ) const {
   shader_program_tessellated.use();
   TextureSet::ScopedBindGuard pin { _textures, shader_program_tessellated }; // RAII
   
   glBindVertexArray(_vao);

   

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

   glPatchParameteri(GL_PATCH_VERTICES, 4);

   glDrawElements(GL_PATCHES,
                  _indices.size(),
                  GL_UNSIGNED_INT,
                  0);

   // bind OpenGL to standard values
   glBindVertexArray(0);
}
