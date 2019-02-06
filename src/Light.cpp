#include "Lights.h"

LightData Light::get_data() const {
   return scene.get_data(_id);
}

void Light::set_data( LightData const &data ) {
   scene.set_data( _id, data );
}

Uint64 Light::get_id() const {
   return _id;
}

Uint64 Light::_generate_id() const {
   static next_id = 0;
   return next_id++;
}
