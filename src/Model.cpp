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

Vector<SharedPtr<Mesh>> const & Model::get_meshes() const {
   return _meshes;
}

void Model::_draw( ShaderProgram &shader_program, Bool should_tessellate ) const {

   for ( auto &e : get_meshes() ){  // for each mesh in the model

      if ( should_tessellate )
         e->_draw_tessellated( shader_program ); // call the mesh's tessellated draw function
      else e->_draw( shader_program );           // call the mesh's draw function


   }
}


SharedPtr<Mesh> Model::_process_mesh( aiMesh *mesh, aiScene const *scene ) {
   Vector<VertexData>  vertices;
   Vector<GLuint>      indices;
   TextureSet          textures;

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


      //if (dot(cross(norm, tang), btan) < 0.0) {
      //   tang = tang * -1.0;
      //}

	  vertex.tangent = { mesh->mTangents[i].x,
						 mesh->mTangents[i].y,
						 mesh->mTangents[i].z };

	  vertex.bitangent = { mesh->mBitangents[i].x,
						   mesh->mBitangents[i].y,
						   mesh->mBitangents[i].z };

     //TODO: Shadow on floor return if we use ">" instead of "<"
     //if (glm::dot(glm::cross(vertex.normal, vertex.tangent), vertex.bitangent) < 0.0) {
     //   vertex.tangent = { -mesh->mTangents[i].x,
     //              -mesh->mTangents[i].y,
     //              -mesh->mTangents[i].z };
     //}

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


   if ( mesh->mMaterialIndex >= 0 ) {
      auto *material { scene->mMaterials[mesh->mMaterialIndex] };

      switch ( material->GetTextureCount(aiTextureType_DIFFUSE) ) {
         case 0: {
            textures.diffuse = std::make_shared<DiffuseTexture>();
         } break;

         case 1: {
            aiString texture_name;
            material->GetTexture( aiTextureType_DIFFUSE, 0, &texture_name );
            FilePath path { FileType::texture, String(texture_name.C_Str()) };
            textures.diffuse = std::make_shared<DiffuseTexture>(path);
         } break;

         default: assert( false && "Engine does not support multiple diffuse maps." );
      }


      switch ( material->GetTextureCount(aiTextureType_SPECULAR) ) {
         case 0: {
            textures.specular = std::make_shared<SpecularTexture>();
         } break;

         case 1: {
            aiString texture_name;
            material->GetTexture( aiTextureType_SPECULAR, 0, &texture_name );
            FilePath path { FileType::texture, String(texture_name.C_Str()) };
            textures.specular = std::make_shared<SpecularTexture>(path);
         } break;

         default: assert( false && "Engine does not support multiple specular maps." );
      }


      switch ( material->GetTextureCount(aiTextureType_NORMALS) ) {
         case 0: {
            textures.normal = std::make_shared<NormalTexture>();
         } break;

         case 1: {
            aiString texture_name;
            material->GetTexture( aiTextureType_NORMALS, 0, &texture_name );
            FilePath path { FileType::texture, String(texture_name.C_Str()) };
            textures.normal = std::make_shared<NormalTexture>(path);
         } break;

         default: assert( false && "Engine does not support multiple normal maps." );
      }

      switch ( material->GetTextureCount(aiTextureType_EMISSIVE) ) {
         case 0: {
            textures.emissive = std::make_shared<EmissiveTexture>();
         } break;

         case 1: {
            aiString texture_name;
            material->GetTexture( aiTextureType_EMISSIVE, 0, &texture_name );
            FilePath path { FileType::texture, String(texture_name.C_Str()) };
            textures.emissive = std::make_shared<EmissiveTexture>(path);
         } break;

         default: assert( false && "Engine does not support multiple emissive maps." );
      }

      switch ( material->GetTextureCount(aiTextureType_DISPLACEMENT) ) {
         case 0: {
            textures.displacement = std::make_shared<DisplacementTexture>();
         } break;

         case 1: {
            aiString texture_name;
            material->GetTexture( aiTextureType_DISPLACEMENT, 0, &texture_name );
            FilePath path { FileType::texture, String(texture_name.C_Str()) };
            textures.displacement = std::make_shared<DisplacementTexture>(path);
         } break;

         default: assert( false && "Engine does not support multiple displacement maps." );
      }


   }

   // TODO: implement more MaterialMaps based on Assimp's aiTextureType enum values
   return std::make_shared<Mesh>( std::move(vertices),
                                  std::move(indices),
                                  std::move(textures) );
}


String Model::get_name() const {
   return _name;
}

