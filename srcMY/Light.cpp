#include "Light.h"

LightData Light::get_data() const {
   return _scene.get_light_data(_id);
}

void Light::set_data( LightData const &data ) {
   _scene.set_light_data( _id, data );
}

Uint64 Light::get_id() const {
   return _id;
}

Uint64 Light::_generate_id() const {
   static Uint64 next_id = 0;
   return next_id++;
}
