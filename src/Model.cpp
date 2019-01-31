#include "Model.h"

#include "misc/defs.h"


Uint32 load_texture_from_file(String const str);

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
   _process_node(scene->mRootNode, scene);
}

void Model::_process_node(aiNode *node, const aiScene *scene)
{
   // process each mesh located at the current node
   for (unsigned int i = 0; i < node->mNumMeshes; i++)
   {
      // the node object only contains indices to index the actual objects in the scene. 
      // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      _mesh_list.push_back(_process_mesh(mesh, scene));
   }
   // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
   for (unsigned int i = 0; i < node->mNumChildren; i++)
   {
      _process_node(node->mChildren[i], scene);
   }

}

Mesh Model::_process_mesh(aiMesh *mesh, const aiScene *scene) {
   Vector<Vertex> a_vertex_list;
   Vector<Uint32> a_index_list;
   Vector<Texture> a_texture_list;

   const Uint32 nmrFaces = mesh->mNumFaces;
   const Uint32 nmrVertices = mesh->mNumVertices;
   //Vi förallokerar Lika många vertricer i a_vertex_lsit
   //för lika många vertricer som finns i "mesh->mNumVertices"   
   a_vertex_list.reserve(nmrVertices); 

   for (Uint32 i = 0; i < nmrVertices; i++) {

      // vi binder en referens till en av de vertricerna vi
      // förallkorat; 
      Vertex &vertex = a_vertex_list[i];
      
      // sen sätter vi värden för Position, Normal och uv
      // på den Vertexen i Listan...
      vertex.normal.x = mesh->mNormals[i].x;
      vertex.normal.y = mesh->mNormals[i].y;
      vertex.normal.z = mesh->mNormals[i].z;

      vertex.position.x = mesh->mVertices[i].x;
      vertex.position.y = mesh->mVertices[i].y;
      vertex.position.z = mesh->mVertices[i].z;
      
      //Innehåller vår mesh textures över huvudtaget?
      if (mesh->mTextureCoords[0]) {
         vertex.uv.x = mesh->mTextureCoords[i]->x;
         vertex.uv.y = mesh->mTextureCoords[i]->y;
      }
      else {
         vertex.uv.x = 0.0f;
         vertex.uv.y = 0.0f;
      }           
         
   }

   //En loop som går igenom Totala antalet Indicies
   //Vi behöver veta då vi vill förallokera storleken av vår
   // a_index_list
   Uint32 nmrOfIndiciesTotal = 0;
   for (Uint32 i = 0; i < nmrFaces; i++) {
      for (Uint32 j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
         nmrOfIndiciesTotal++;
      }
   }
   
   a_index_list.reserve(nmrOfIndiciesTotal);

   for (Uint32 i = 0; i < nmrFaces; i++) {
      for (Uint32 j = 0; j < mesh->mFaces[j].mNumIndices; j++) {
         //Då vi allokerat index-listan kan vi köra följande rad
         a_index_list[j] = mesh->mFaces[j].mIndices[j];
      }
   }
   
   aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
   Vector<Texture> diffuseMaps = _load_material_textures(material, 
                                                         aiTextureType_DIFFUSE,
                                                         "tex_diff");
   Vector<Texture> specularMaps = _load_material_textures(material,
                                                         aiTextureType_SPECULAR,
                                                         "tex_spec");
   Vector<Texture> normalMaps = _load_material_textures(material,
                                                         aiTextureType_NORMALS,
                                                         "tex_norm");
   //Resevera allt en gång istället för, för varje map
   a_texture_list.reserve(diffuseMaps.size() + specularMaps.size() + normalMaps.size());
   for(auto &e :diffuseMaps){
      a_texture_list.push_back(e);
   }
   for (auto &e : specularMaps){
      a_texture_list.push_back(e);
   }
   for (auto &e : normalMaps){
      a_texture_list.push_back(e);
   }   

   //Implement more MaterialMaps based on ASSIMP's aiTextureType's (Enums) 
}
   
Vector<Texture> Model::_load_material_textures(aiMaterial *material, aiTextureType type, String type_name) {
   Vector<Texture> texture_list;
   
   for (Uint32 i = 0; i < material->GetTextureCount(type); i++) {

      aiString str;
      material->GetTexture(type, i, &str);

      Texture texture;
      texture.id = TextureFromFile;
      texture.type = ;
      texture.path = ;

   }
}

Uint32 load_texture_from_file(String const &file_name) {

   String path_to_file = config::model_path + file_name;

   Uint32 texture_id;
   glGenTextures(1, &texture_id);

   Uint32 width, height, nr_components;
   Unsigned


}