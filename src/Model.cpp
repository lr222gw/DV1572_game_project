#include "Model.h"

//#include "misc/defs.h"


Uint32 load_texture_from_file( FilePath path ) {

   Uint32  texture_id;
   glGenTextures( 1, &texture_id) ;

   Int32  width, height, channel_count;
   //Byte istället för Unsigned Char, samma storlek...
   Uint8 *image_data = stbi_load( path.relative_path().c_str(), &width, &height, &channel_count, 0 );

   if (image_data) {

      GLenum format;
      if (1 == channel_count)
         format = GL_RED;
      else if (3 == channel_count)
         format = GL_RGB;
      else if (4 == channel_count)
         format = GL_RGBA;

      glBindTexture( GL_TEXTURE_2D, texture_id );

      glTexImage2D( GL_TEXTURE_2D,
                    0,
                    format,
                    width,
                    height,
                    0,
                    format,
                    GL_UNSIGNED_BYTE,
                    image_data );

      glGenerateMipmap( GL_TEXTURE_2D );

      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   }
   else {
      //TODO: FMT bibliotek här, sträng hantering?
      assert( false && String("Error While Loading Texture using \"load_texture_from_file\": " + path.relative_path() + " \n").c_str() );
   }

   stbi_image_free( image_data );

   return texture_id;
}

Model::Model( String const &filename ) {
   
    _load_model( filename );
  
}

void Model::_load_model( String const &filename ) {

   // Använder Assimps Importer klass för att importera en Modelfil.
   Assimp::Importer importer;
   //Vi får en Scene, den innehåller alla data för modellen; Normaler, Vertricer, matrial, etc
                  //aiProcess_Triangulate       : Om Modellen inte består av trianglar så gör den allt till trianglar
                  //aiProcess_FlipUVs           : Flippar UV'n på Y-axeln     
                  // Finns fler Post-Processing Options för Assimp
   aiScene const *  scene = importer.ReadFile( filename, aiProcess_Triangulate | aiProcess_FlipUVs );

   bool encounteredError = !scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode;
   if (encounteredError) {// if is Not Zero
      assert(f alse && String("Error; Assimp: " + String(importer.GetErrorString()) + "\n").c_str() );            
   }

   _process_node(scene->mRootNode, scene);
}

void Model::_process_node( aiNode *node,  aiScene const *scene ) {
   //Gå igenom varje Node (eller childNodes) Mesh, ProcessMesh för varje mesh
   for ( Uint32 i = 0;  i < node->mNumMeshes;  ++i ) {      
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      _mesh_list.push_back( _process_mesh(mesh, scene) );
   }

   //När Noden's Mesh är behandlad gör vi samma med Nodens ChildNodes
   for ( Uint32 i = 0;  i < node->mNumChildren;  ++i ) {
      _process_node(node->mChildren[i], scene);
   }
   
}

void Model::draw( ShaderProgram &shader_program ) {
   for ( auto &e : get_mesh_list() ) {
      e._draw( shader_program );
   }
}

Mesh Model::_process_mesh( aiMesh *mesh, aiScene const *scene ) {
   Vector<Vertex>   vertices;
   Vector<Uint32>   indices;
   Vector<Texture>  textures;

   const Uint32  face_count = mesh->mNumFaces;
   const Uint32  vert_count = mesh->mNumVertices;
   //Vi förallokerar Lika många vertricer i a_vertex_lsit
   //för lika många vertricer som finns i "mesh->mNumVertices"   
   vertices.reserve(vert_count); 

   for ( Uint32 i = 0;  i < vert_count;  ++i ) {

      // vi binder en referens till en av de vertricerna vi
      // förallkorat; 
      Vertex vertex;
      // sen sätter vi värden för Position, Normal och uv
      // på den Vertexen i Listan...

      vertex.position  = { mesh->mVertices[i].x ,
                           mesh->mVertices[i].y ,
                           mesh->mVertices[i].z };

      vertex.normal    = { mesh->mNormals[i].x ,
                           mesh->mNormals[i].y ,
                           mesh->mNormals[i].z };

      //Innehåller vår mesh textures över huvudtaget?
      if ( mesh->mTextureCoords[0] ) {
         vertex.uv  = { mesh->mTextureCoords[0][i].x ,
                        mesh->mTextureCoords[0][i].y };
      }
      else {
         vertex.uv.x = 0.0f;
         vertex.uv.y = 0.0f;
      }           
      
      vertices.push_back( vertex );
   }

   //En loop som går igenom Totala antalet Indicies
   //Vi behöver veta då vi vill förallokera storleken av vår
   // a_index_list
   //Uint32 nmrOfIndiciesTotal = 0;
   //for (Uint32 i = 0; i < nmrFaces; i++) {
   //   for (Uint32 j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
   //      nmrOfIndiciesTotal++;
   //   }
   //}
   
   //a_index_list.reserve(nmrOfIndiciesTotal);
   //Uint32 counter = 0; 
   for ( Uint32 i = 0;  i < face_count;  ++i ) {
      for ( Uint32 j = 0; j < mesh->mFaces[i].mNumIndices; ++j ) {
         //Då vi allokerat index-listan kan vi köra följande rad         
         indices.push_back( mesh->mFaces[i].mIndices[j] );
      }
   }
   
   if ( mesh->mMaterialIndex >= 0 ) {

      aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

      // load diffuse maps:
      Vector<Texture> diffuse_maps = _load_material_textures( material,
                                                              aiTextureType_DIFFUSE,
                                                              "tex_diff");
      textures.insert( textures.end(), diffuse_maps.begin(), diffuse_maps.end() );

      // load specular maps:
      Vector<Texture> specular_maps = _load_material_textures( material,
                                                               aiTextureType_SPECULAR,
                                                               "tex_spec");
      textures.insert( textures.end(), specular_maps.begin(), specular_maps.end() );

      // load normal maps:
      Vector<Texture> normal_maps = _load_material_textures( material,
                                                             aiTextureType_NORMALS,
                                                             "tex_norm");
      textures.insert( textures.end(), normal_maps.begin(), normal_maps.end() );
   }

   //Resevera allt en gång istället för, för varje map
   //a_texture_list.reserve(diffuseMaps.size() + specularMaps.size() + normalMaps.size());
   //for(auto &e :diffuseMaps){
   //   a_texture_list.push_back(e);
   //}
   //for (auto &e : specularMaps){
   //   a_texture_list.push_back(e);
   //}
   //for (auto &e : normalMaps){
   //   a_texture_list.push_back(e);
   //}   

   //Implement more MaterialMaps based on ASSIMP's aiTextureType's (Enums) 
   return Mesh( vertices, indices, textures );
}

//TODO: Bryta ut "_load_material_textures" och "load_texture_from_file" till TextureHandler
// Undvik att ladda in en textur som redan är inladdad... 
Vector<Texture> Model::_load_material_textures( aiMaterial *material, aiTextureType type, String type_name ) {
   Vector<Texture> texture_list;
   
   for ( Uint32 i = 0;  i < material->GetTextureCount(type);  ++i ) {

      aiString str;
      material->GetTexture( type, i, &str );      

      GLboolean skip = false;

      String path_to_file = config::model_path + String(str.C_Str());
      FilePath path { FileType::texture, String(str.C_Str()) };
      
      Texture texture;
      texture.id = load_texture_from_file(path);
      texture.type = type_name;
      texture.path = path.relative_path();
      texture_list.push_back(texture);
   }

   return texture_list;
}

