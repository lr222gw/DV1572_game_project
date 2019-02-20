#pragma once

#include "glm/glm.hpp"
#include "misc/defs.h"
#include "Texture.h"
#include "ShaderProgram.h"


struct VertexData {
   glm::vec3  position; // vertex local coordinate
   glm::vec3  normal;   // vertex normal
   glm::vec3  tangent;	// tangent
   glm::vec3  bitangent;// bitangent
   glm::vec2  uv;       // texture coordinate
};

class Mesh {
   friend class Model; // gives Model access to Mesh's private members

/*--------------- class member functions & operators ------------*/
public:
   Mesh( Vector<VertexData>         &&vertices,
         Vector<GLuint>             &&indices,
         Vector<SharedPtr<Texture>> &&textures )
   :
      _vertices  ( std::move(vertices) ),
      _indices   ( std::move(indices)  ),
      _textures  ( std::move(textures) )
   {
      _initialize_mesh();
   }

   Mesh( Mesh const & ) = delete;

   Mesh( Mesh &&other ):
      _vertices  ( std::move(other._vertices) ),
      _indices   ( std::move(other._indices)  ),
      _textures  ( std::move(other._textures) )
   {}

private:
   void _draw( ShaderProgram & ) const;
   void _initialize_mesh();

/*--------------- class member variables & constants ------------*/
private:
   Vector<VertexData>          _vertices;
   Vector<GLuint>              _indices;
   Vector<SharedPtr<Texture>>  _textures;

   GLuint  _vao,
           _vbo,
           _ebo;
};

