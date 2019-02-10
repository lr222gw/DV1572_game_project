#include "Model.h"

Model::Model( String const &filename ):
   _name ( filename )
{
    _load_model( filename );
}


Uint32 load_texture_from_file( FilePath path ) {
   Uint32  texture_id;
   glGenTextures( 1, &texture_id );

   Int32  width,
          height,
          channel_count;

   Uint8 *image_data = stbi_load( path.relative_path().c_str(),
                                  &width,
                                  &height,
                                  &channel_count,
                                  0 );

   if ( image_data ) {

      GLenum format;
      if      ( 1 == channel_count )
         format = GL_RED;
      else if ( 3 == channel_count )
         format = GL_RGB;
      else if ( 4 == channel_count )
         format = GL_RGBA;
      else assert( false && "Unexpected texture format channel count." );

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

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_WRAP_S,
                       GL_REPEAT );

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_WRAP_T,
                       GL_REPEAT );

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_MIN_FILTER,
                       GL_LINEAR_MIPMAP_LINEAR );

      glTexParameteri( GL_TEXTURE_2D,
                       GL_TEXTURE_MAG_FILTER,
                       GL_LINEAR );
   } // TODO: use {fmt} here for formatting?
   else assert( false && String( "Error while trying to load texture from file using:"
                                 "load_texture_from_file( "+ path.relative_path() +"). ").c_str() );

   stbi_image_free( image_data );

   return texture_id;
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
                                          aiProcess_Triangulate | aiProcess_FlipUVs );

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
      _mesh_list.push_back( _process_mesh(mesh, scene) ); // and process it
   }

   for ( Uint32 i = 0;  i < node->mNumChildren;  ++i ) // for each child mesh,
      _process_node( node->mChildren[i], scene );      // extract and proccess it
}


void Model::draw( ShaderProgram &shader_program ) {
   for ( auto &e : get_mesh_list() )  // for each mesh in the model
      e._draw( shader_program );      // call the mesh's draw function
}


Mesh Model::_process_mesh( aiMesh *mesh, aiScene const *scene ) {
   Vector<VertexData>   vertices;
   Vector<GLuint>       indices;
   Vector<TextureData>  textures;

   const Uint32  face_count = mesh->mNumFaces;
   const Uint32  vert_count = mesh->mNumVertices;

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
      Vector<TextureData> diffuse_maps = _load_material_textures( material,
                                                                  aiTextureType_DIFFUSE,
                                                                  "tex_diff");

      textures.insert( textures.end(), diffuse_maps.begin(), diffuse_maps.end() );

      // load specular maps:
      Vector<TextureData> specular_maps = _load_material_textures( material,
                                                                   aiTextureType_SPECULAR,
                                                                   "tex_spec");

      textures.insert( textures.end(), specular_maps.begin(), specular_maps.end() );

      // load normal maps:
      Vector<TextureData> normal_maps = _load_material_textures( material,
                                                                 aiTextureType_NORMALS,
                                                                 "tex_norm");

      textures.insert( textures.end(), normal_maps.begin(), normal_maps.end() );
   }

   // TODO: implement more MaterialMaps based on Assimp's aiTextureType enum values
   return Mesh( vertices, indices, textures );
}


String Model::get_name() const {
   return _name;
}


// TODO: Bryta ut "_load_material_textures" och "load_texture_from_file" till TextureHandler
// Undvik att ladda in en textur som redan är inladdad...
Vector<TextureData> Model::_load_material_textures( aiMaterial    *material,
                                                    aiTextureType  type,
                                                    String         type_name ) {
   Vector<TextureData> textures;

   for ( Uint32 i = 0;  i < material->GetTextureCount(type);  ++i ) {
      aiString model_name;
      material->GetTexture( type, i, &model_name );

      FilePath path { FileType::texture, String(model_name.C_Str()) };

      TextureData texture;
      texture.id   = load_texture_from_file( path );
      texture.type = type_name;
      texture.path = path.relative_path();
      textures.push_back( texture );
   }

   return textures;
}
