#pragma once

#include "glm/glm.hpp"
#include "misc/defs.h"
#include "ShaderProgram.h"


struct Vertex {
   glm::vec3  position; // vertex local coordinate
   glm::vec3  normal;   // vertex normal
   glm::vec2  uv;       // texture coordinate
};


struct Texture {
   GLuint  id;   // texture ID
   String  type; // texture type
   String  path; // texture path
};



class Mesh {
   friend class Model; // gives Model access to Mesh's private members

/*--------------- class member functions & operators ------------*/
public:
   Mesh(Vector<Vertex> vert_list, Vector<GLuint> index_list, Vector<Texture> texture_list) :
      vertex_list(vert_list), index_list(index_list), texture_list(texture_list) {
         _initialize_mesh();
   };

private:
   void _draw(ShaderProgram &shaderProgram);
   void _initialize_mesh();

/*--------------- class member variables & constants ------------*/
public:
   Vector<Vertex>    vertex_list;   // TODO: make private?
   Vector<GLuint>    index_list;    // TODO: make private?
   Vector<Texture>   texture_list;  // TODO: make private?

private:
   GLuint  _vao,
           _vbo,
           _ebo;
};
