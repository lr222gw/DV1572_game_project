#pragma once

#include "glm/glm.hpp"
#include "misc/defs.h"
#include "ShaderProgram.h"

struct Vertex {
   glm::vec3 position;
   glm::vec3 normal;
   glm::vec2 uv;//texCoord
};

struct Texture {
   Uint32 id;
   String type;
   String path;
};

class Mesh {
   friend class Model; //Ger oss tillgång till Mesh's privata
                       //funktioner från Model klassen

public:
   Vector<Vertex>    vertex_list;
   Vector<Uint32>    index_list;
   Vector<Texture>   texture_list;

   Mesh(Vector<Vertex> vert_list, Vector<Uint32> index_list, Vector<Texture> texture_list) :
      vertex_list(vert_list), index_list(index_list), texture_list(texture_list) {
         _initialize_mesh();
   };

private:
   GLuint _vao,
          _vbo,
          _ebo;

   void _draw(ShaderProgram *shaderProgram);
   void _initialize_mesh();

};