#pragma once

#include "glm/glm.hpp"
#include "misc/defs.h"
#include "ShaderProgram.h"

struct Vertex {
   glm::vec3 Position;
   glm::vec3 Normal;
   glm::vec2 TexCoord;
};

struct Texture {
   Uint32 id;
   String type;

};

class Mesh {
public:
   Vector<Vertex>    Vertex_list;
   Vector<Uint32>    Index_list;
   Vector<Texture>   Texture_list;

   Mesh(Vector<Vertex> vert_list, Vector<Uint32> index_list, Vector<Texture> texture_list) :
      Vertex_list(vert_list), Index_list(index_list), Texture_list(texture_list) {
         initializeMesh();
   };

   void draw(ShaderProgram shaderProgram);

private:

   GLuint VAO,
          VBO,
          EBO;

   void initializeMesh();

};