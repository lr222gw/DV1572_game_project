#include "Light.h"

// LightData Light::get_data() const {
//    return _scene.get_light_data(_id);
// }
//
// void Light::set_data( LightData const &data ) {
//    _scene.set_light_data( _id, data );
// }
//
// Uint64 Light::get_id() const {
//    return _id;
// }
//
// Uint64 Light::_generate_id() const {
//    static Uint64 next_id = 0;
//    return next_id++;
// }

Light::~Light() { _on_destruction_callback( id ); }

void Light::set_position(    Vec3       &&position    ) {
   _data.position    = std::move( position  );
   _on_change_callback( id );
}

void Light::set_direction(   Vec3       &&direction   ) {
   _data.direction   = std::move( direction );
   _on_change_callback( id );
}

void Light::set_color(       Vec3       &&color       ) {
   _data.color       = std::move( color     );
   _on_change_callback( id );
}

void Light::set_position(    Vec3 const  &position    ) {
   _data.position    = position              ;
   _on_change_callback( id );
}

void Light::set_direction(   Vec3 const  &direction   ) {
   _data.direction   = direction             ;
   _on_change_callback( id );
}

void Light::set_color(       Vec3 const  &color       ) {
   _data.color       = color                 ;
   _on_change_callback( id );
}

void Light::set_intensity(   Float32      intensity   ) {
   _data.intensity   = intensity             ;
   _on_change_callback( id );
}

void Light::set_radius(      Float32      radius      ) {
   _data.radius      = radius                ;
   _on_change_callback( id );
}

void Light::set_degree(      Float32      degree      ) {
   _data.degree      = degree                ;
   _on_change_callback( id );
}

void Light::set_specularity( Float32      specularity ) {
   _data.specularity = specularity           ;
   _on_change_callback( id );
}

void Light::set_type(        Light::Type  type        ) {
   _data.type        = type                  ;
   _on_change_callback( id );
}


Vec3                Light::get_position()    const { return _data.position;    }
Vec3                Light::get_direction()   const { return _data.direction;   }
Vec3                Light::get_color()       const { return _data.color;       }
Float32             Light::get_intensity()   const { return _data.intensity;   }
Float32             Light::get_radius()      const { return _data.radius;      }
Float32             Light::get_degree()      const { return _data.degree;      }
Float32             Light::get_specularity() const { return _data.specularity; }
Light::Type         Light::get_type()        const { return _data.type;        }
Light::Data const & Light::get_data()        const { return _data;             }
Uint64              Light::get_id()          const { return id;                }
