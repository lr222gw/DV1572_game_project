#pragma once
#include "misc/defs.h"
#include "Mesh.h"

class Model {
public:
   Model(String const &filename);

   Vector<Texture> texture_list;	
   Vector<Mesh> mesh_list;

private:
   void _load_model(String const &filename);

};