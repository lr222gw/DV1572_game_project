#pragma once

#include "glm/glm.hpp"
#include "misc/defs.h"
#include "ShaderProgram.h"


struct VertexData {
   glm::vec3  position; // vertex local coordinate
   glm::vec3  normal;   // vertex normal
   glm::vec3  tangent;	// tangent
   glm::vec3  bitangent;// bitangent
   glm::vec2  uv;       // texture coordinate
};


struct TextureData {
   GLuint  id;   // texture ID
   String  type; // texture type
   String  path; // texture path
};


class Mesh {
   friend class Model; // gives Model access to Mesh's private members

/*--------------- class member functions & operators ------------*/
public:
   Mesh( Vector<VertexData>   vertex_list,
         Vector<GLuint>       index_list,
         Vector<TextureData>  texture_list )
   :
      _vertex_list  ( vertex_list  ),
      _index_list   ( index_list   ),
      _texture_list ( texture_list )
   {
         _initialize_mesh();
   }

private:
   void _draw( ShaderProgram & );
   void _initialize_mesh();

/*--------------- class member variables & constants ------------*/
private:
   Vector<VertexData>    _vertex_list;
   Vector<GLuint>        _index_list;
   Vector<TextureData>   _texture_list;

   GLuint  _vao,
           _vbo,
           _ebo;
};

