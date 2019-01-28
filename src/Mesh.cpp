#include "Mesh.h"

//Mesh::Mesh(Vector<Vertex> vertex_list, Vector<Uint32> index_list, Vector<Texture> texture_list) {
//
//}

void Mesh::initializeMesh() {

   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   glGenBuffers(1, &EBO);

   glBindVertexArray(VAO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);

   glBufferData(GL_ARRAY_BUFFER, Vertex_list.size() * sizeof(Vertex), &Vertex_list[0], GL_STATIC_DRAW);
   
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, Index_list.size() * sizeof(Uint32), &Index_list[0], GL_STATIC_DRAW);

   //Attributes för Vertex Strukten; Position, Normal och TexturKoordinat
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

   glBindVertexArray(0);
}

void Mesh::draw(ShaderProgram shaderProgram) {

}

