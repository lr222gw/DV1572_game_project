#pragma once
#include "misc/defs.h"
#include "Mesh.h"

class Model {
public:
   Model(String const &filename);

   vector<Texture> texture_list;	
   vector<Mesh> mesh_list;

private:
   void _load_model(String const &filename);

};