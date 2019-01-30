#include "Model.h"

#include "misc/defs.h"



Model::Model(String const &filename) {
   
    _load_model(filename);
  
}

void Model::_load_model(String const &filename) {

   // read file via ASSIMP
   Assimp::Importer importer;
   const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
   // check for errors
   if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
   {
      std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
      return;
   }
   // retrieve the directory path of the filepath
   //directory = path.substr(0, path.find_last_of('/'));

   // process ASSIMP's root node recursively
   _processNode(scene->mRootNode, scene);
}

Mesh Model::_processNode(aiNode *node, const aiScene *scene)
{
   // process each mesh located at the current node
   for (unsigned int i = 0; i < node->mNumMeshes; i++)
   {
      // the node object only contains indices to index the actual objects in the scene. 
      // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      mesh_list.push_back(_processNode(mesh, scene));
   }
   // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
   for (unsigned int i = 0; i < node->mNumChildren; i++)
   {
      _processNode(node->mChildren[i], scene);
   }

}