#include "Mesh.h"

//Mesh::Mesh(Vector<Vertex> vertex_list, Vector<Uint32> index_list, Vector<Texture> texture_list) {
//
//}

void Mesh::_initialize_mesh() {

   glGenVertexArrays(1, &_vao);
   glGenBuffers(1, &_vbo);
   glGenBuffers(1, &_ebo);

   glBindVertexArray(_vao);
   glBindBuffer(GL_ARRAY_BUFFER, _vbo);

   glBufferData(GL_ARRAY_BUFFER, vertex_list.size() * sizeof(Vertex), &vertex_list[0], GL_STATIC_DRAW);
   
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_list.size() * sizeof(Uint32), &index_list[0], GL_STATIC_DRAW);

   //Attributes för Vertex Strukten; Position, Normal och TexturKoordinat
   //Position
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
                      //0 indicates that this is the first Element of a Struct, 
   //Normal
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
                      //1 indicates that this is the second element of  the struct
   //TextureKoordinat
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
                     //2 indicates that this is the third element of  the struct
   glBindVertexArray(0);
}

void Mesh::_draw(ShaderProgram &shader_program) {
   GLuint diffuseNr = 1;
   GLuint specularNr = 1;
   GLuint normalNr = 1;

   for (GLuint i = 0; i < this->texture_list.size(); i++) {
      
      //ställa in vilken Textur vi jobbar på, vi kan högst ha 32 (eller 16?)
      glActiveTexture(GL_TEXTURE0 + i); 
         // varv 1; i = 0; GL_TEXTURE0 + i = GL_TEXTURE0;;; varv 2; i = 1 ; GL_TEXTURE0 + i = GL_TEXTURE1, osv.

      //Vi hämtar datan 
      std::stringstream ss; 
      String number;
      String name = this->texture_list[i].type; 

      if ("tex_diff" == name) {
         ss << diffuseNr++;
      }
      else if ("tex_spec" == name) {
         ss << specularNr++;
      }
      else if ("tex_norm" == name) {
         ss << normalNr++;
      }

      number = ss.str();
      
      glUniform1i(glGetUniformLocation(shader_program.getProgramLoc(), (name + number).c_str()), i);
      glBindTexture(GL_TEXTURE_2D, this->texture_list[i].id);
   }

   glUniform1f(glGetUniformLocation(shader_program.getProgramLoc(), "material.shininess"), 16.0f);

   glBindVertexArray(this->_vao);
   glDrawElements(GL_TRIANGLES, this->index_list.size(), GL_UNSIGNED_INT, 0);

   //Bind OpenGL till standard värderna...
   glBindVertexArray(0);
   for (GLuint i = 0; i < this->texture_list.size(); i++) {

      glActiveTexture(GL_TEXTURE0+i);
      glBindTexture(GL_TEXTURE_2D,0);

   }

}

