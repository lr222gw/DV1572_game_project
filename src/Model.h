#pragma once
#include "misc/defs.h"
#include "Mesh.h"

#include "Config.h"

#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

class Model {
public:
   Model( String const &filename );

   Vector<TextureData> &get_texture_list() {
      return _texture_list;
   }

   Vector<Mesh> &get_mesh_list() {
      return _mesh_list;
   }

   String get_name() const;

   void draw( ShaderProgram & );

private:
   String const         _name;
   Vector<TextureData>  _texture_list;
   Vector<Mesh>         _mesh_list;

   void _process_node( aiNode *, const aiScene * );
   Mesh _process_mesh( aiMesh *, const aiScene * );
   void _load_model( String const &filename );

   //TODO: bryta ut till egenklass f�r textur hantering?
   Vector<TextureData> _load_material_textures( aiMaterial *,
                                                aiTextureType,
                                                String  type_name );
};