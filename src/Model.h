#pragma once
#include "misc/defs.h"
#include "Mesh.h"

#include "Config.h"

#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

class Model {
public:
   Model(String const &filename);

   Vector<Texture> &get_texture_list() {
      return _texture_list;
   }

   Vector<Mesh> &get_mesh_list() {
      return _mesh_list;
   }   

   void draw(ShaderProgram *shaderProgram);

private:
   Vector<Texture> _texture_list;
   Vector<Mesh> _mesh_list;

   void _process_node(aiNode *node, const aiScene *scene);
   Mesh _process_mesh(aiMesh *mesh, const aiScene *scene);
   void _load_model(String const &filename);

   //TODO: bryta ut till egenklass för textur hantering?
   Vector<Texture> _load_material_textures(aiMaterial *material, aiTextureType type, String type_name);
};