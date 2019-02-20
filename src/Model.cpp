#include "Model.h"
#include "Texture.h"

Model::Model( String const &filename ):
   _name ( filename )
{
    _load_model( filename );
}

void Model::_load_model( String const &filename ) {
   Assimp::Importer importer; // using Assimp's importer class
   //
   // the importer loads a scene including all of the model's data:
   //    + vertex normals
   //    + vertices
   //    + materials
   //    etc.
   //
   // aiProcess_Triangulate flag:
   //    converts the mesh to triangles if it contains non-tri faces (e.g. quads)
   //
   // aiProcess_FlipUVs flag:
   //    flips UV coordinates along the Y-axis
   //
   // Assimp also provides various other post-processing options that we don't use

   auto const *scene = importer.ReadFile( filename,
                                          aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

   Bool encountered_error =  !scene
                          || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
                          || !scene->mRootNode;

   if ( encountered_error ) {
      assert( false && String( "[ERROR]  Assimp: "
                               +String(importer.GetErrorString()) + "\n").c_str() );
   }

   _process_node( scene->mRootNode, scene );
}

void Model::_process_node( aiNode *node,  aiScene const *scene ) {
   for ( Uint32 i = 0;  i < node->mNumMeshes;  ++i ) {    // for each mesh,
      auto *mesh = scene->mMeshes[node->mMeshes[i]];      // extract the mesh
      _meshes.emplace_back( _process_mesh(mesh, scene) ); // and process it
   }

   for ( Uint32 i = 0;  i < node->mNumChildren;  ++i ) // for each child mesh,
      _process_node( node->mChildren[i], scene );      // extract and proccess it
}


void Model::draw( ShaderProgram &shader_program ) const {
   for ( auto &e : get_meshes() )  // for each mesh in the model
      e->_draw( shader_program );   // call the mesh's draw function
}


SharedPtr<Mesh> Model::_process_mesh( aiMesh *mesh, aiScene const *scene ) {
   Vector<VertexData>          vertices;
   Vector<GLuint>              indices;
   Vector<SharedPtr<Texture>>  textures;

   Uint32 const  face_count = mesh->mNumFaces;
   Uint32 const  vert_count = mesh->mNumVertices;

   // pre-allocate as many vertices as the mesh contains
   vertices.reserve(vert_count);
   for ( Uint32 i = 0;  i < vert_count;  ++i ) {
      VertexData vertex; // our translated vertex

      // extract position of vertex:
      vertex.position  = { mesh->mVertices[i].x ,
                           mesh->mVertices[i].y ,
                           mesh->mVertices[i].z };

      // extract normal of vertex:
      vertex.normal    = { mesh->mNormals[i].x ,
                           mesh->mNormals[i].y ,
                           mesh->mNormals[i].z };

	  vertex.tangent = { mesh->mTangents[i].x,
						 mesh->mTangents[i].y,
						 mesh->mTangents[i].z };

	  vertex.bitangent = { mesh->mBitangents[i].x,
						   mesh->mBitangents[i].y,
						   mesh->mBitangents[i].z };

      // check whether mesh contains textures:
      if ( mesh->mTextureCoords[0] ) {  // if it does, extract UV-coords
         vertex.uv  = { mesh->mTextureCoords[0][i].x,
                        mesh->mTextureCoords[0][i].y };
      }
      else { // otherwise set them to zero
         vertex.uv.x = 0.0f;
         vertex.uv.y = 0.0f;
      }

      // add the vertex to our vector of vertices
      vertices.push_back( vertex );
   }

   // pre-allocate space for the estimated minimum index count
   indices.reserve( face_count * 3 );
   for ( Uint32 i = 0;  i < face_count;  ++i ) {
      auto &current_face = mesh->mFaces[i];
      for ( Uint32 j = 0;  j < current_face.mNumIndices;  ++j ) {
         indices.push_back( current_face.mIndices[j] );
      }
   }

   // TODO: profile textures.reserve(...)
   if ( mesh->mMaterialIndex >= 0 ) {
      auto *material { scene->mMaterials[mesh->mMaterialIndex] };

      // load diffuse maps:
      Vector<SharedPtr<Texture>> diffuse_maps = _load_material_textures( material,
                                                                         aiTextureType_DIFFUSE );
      for ( auto &e : diffuse_maps )
         textures.emplace_back( e );


      // load specular maps:
      Vector<SharedPtr<Texture>> specular_maps = _load_material_textures( material,
                                                                          aiTextureType_SPECULAR );
      for ( auto &e : specular_maps )
         textures.emplace_back( e );


      // load normal maps:
      Vector<SharedPtr<Texture>> normal_maps = _load_material_textures( material,
                                                                        aiTextureType_NORMALS );
      for ( auto &e : normal_maps )
         textures.emplace_back( e );
   }

   // TODO: implement more MaterialMaps based on Assimp's aiTextureType enum values
   return std::make_shared<Mesh>( std::move(vertices),
                                  std::move(indices),
                                  std::move(textures) );
}


String Model::get_name() const {
   return _name;
}


// TODO: Bryta ut "_load_material_textures" och "load_texture_from_file" till TextureHandler
// Undvik att ladda in en textur som redan är inladdad...
Vector<SharedPtr<Texture>> Model::_load_material_textures( aiMaterial    *material,
                                                           aiTextureType  type )
{
   Vector<SharedPtr<Texture>> textures;

   Texture::Type our_type;
   switch ( type ) {
      case aiTextureType_DIFFUSE:   our_type = Texture::Type::diffuse;  break;
      case aiTextureType_SPECULAR:  our_type = Texture::Type::specular; break;
      case aiTextureType_NORMALS:   our_type = Texture::Type::normal;   break;
      default: assert( false && "Unknown texture type encountered!" );
   }

   for ( Uint32 i = 0;  i < material->GetTextureCount(type);  ++i ) {
      aiString model_name;
      material->GetTexture( type, i, &model_name );

      FilePath path { FileType::texture, String(model_name.C_Str()) };

      textures.emplace_back( std::make_shared<Texture>(path, our_type) );
   }

   return textures;
}
