#pragma once
#include "misc/defs.h"
#include "Mesh.h"

#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

class Model {
public:
   Model(String const &filename);

   Vector<Texture> texture_list;	
   Vector<Mesh> mesh_list;

private:
   Mesh _processNode(aiNode *node, const aiScene *scene);
   void _load_model(String const &filename);

};