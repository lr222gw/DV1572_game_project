#pragma once
#include "misc/defs.h"
#include "Mesh.h"
#include "Texture.h"

#include "Config.h"

#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

class Model {
   friend class ModelInstance; // gives Model access to Mesh's private members
public:
   Model( String const &filename );

   Model( Model const &  ) = delete;
   Model( Model       && ) = delete;

   ~Model() {
      // TODO: free textures and other bound data
      // textures ought to be freed by GC automatically
   }

   Model & operator=( Model const  & ) = delete;
   Model & operator=( Model       && ) = delete;

   [[nodiscard]] Vector<SharedPtr<Mesh>> const & get_meshes() const;
   [[nodiscard]] String get_name() const;

private:
   String const             _name;
   Vector<SharedPtr<Mesh>>  _meshes;

   void                          _process_node( aiNode *, const aiScene * );
   [[nodiscard]] SharedPtr<Mesh> _process_mesh( aiMesh *, const aiScene * );
   void                          _load_model(   String const &filename );
   void                          _draw( ShaderProgram &, Bool should_tessellate ) const;
};
