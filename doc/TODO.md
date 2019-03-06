


class Light                    { /*...*/ };
class DirectionalLight : Light { /*...*/ };
class PointLight       : Light { /*...*/ };

class Shadowcaster {
public:
   Shadowcaster( SharedPtr<Light> source ) {}

private:
   SharedPtr<Light> _source;
   Mat4             _light_matrix;
};


SceneManger::set_shadowcasting( Light const *light ) {
   if ( LightType::directional == light->get_type() )
      /*Vector<Shadowcaster>*/ _shadowcasters.emplace_back( light );
   else // do nothing
}










class ParticleSystem {
public:

   struct ParticleData {
   public:
      // member data fields:
      constexpr Uint32       maximum_count = Config::particle_max_count; // TODO: rename to make it more obvious that the limit is on a per ParticleSystem basic
      Uint32                 current_count;
   // Vec3[max_capacity]     color;
      Vec3[max_capacity]     position;
      Float32[max_capacity]  opacity;
      Float32[max_capacity]  scale;
      Uint64[max_capacity]   id;


   // member data functions:
      void create( Vec3         *position,
                   Float32      *opacity,
                   Float32      *scale,
                   Uint32 const  count )
      {
         Uint32 n;
         // avoid exceeding max capacity
         if ( current_count + count > max_capacity )
            n = max_capacity - current_count;
         else
            n = count;

         // create n particles (n = count)
         for ( Uint32 i = 0;  i < count;  ++i )
            position [current_count] = position[i];
            opacity  [current_count] =  opacity[i];
            scale    [current_count] =    scale[i];
            id       [current_count] = _generate_id();
            ++current_count;
         }
      }

      void destroy( Uint32 particle_index ) {
         --current_count; // <- is now the index of the last element
         // fill any potential gap by swapping the previously last index's data in:
         std::swap( & position[particle_index], & position[current_count] );
         std::swap( &  opacity[particle_index], &  opacity[current_count] );
         std::swap( &    scale[particle_index], &    scale[current_count] );
         std::swap( &       id[particle_index], &       id[current_count] );
         // the potential gap has been filled,
         // current_count is now a sentinel index
      }

   private:
      // member data fields:
      Uint64 _next_id = 0;

      // member data functions:
      Uint64 _generate_id() {
         return _next_id++;
      }
   };

   ParticleSystem( /*TODO: lambda algorithm */ ): _algorithm( algorithm ) {}

   void update( Float32 delta_t_s ) {
    _algorithm( _data, delta_t_s );
    /* TODO: send new data to shaders */
   }

private:


   auto lambda = [] ( ParticleData &particles, Float32 delta_t_s );






   /*TODO*/       _algorithm;
   ParticleData   _data;
};







billboard physics:


geometry shader:

uint                 max_capacity
vec3[max_capacity]   positions
float[max_capacity]  opacity
float[max_capacity]  scales
uint                 particle_count

uniform sampler2D tex_diff;
uniform sampler2D tex_spec;
uniform sampler2D tex_norm;
uniform sampler2D tex_emit;



particle function:


[] ( Uint32        &count,
     Vec3          *positions,
     Float32       *scales,
     Float32       *opacities,
     Float32 const  delta_t_s )
{
   for_( Uint32 i = 0;  i < count;  ++i ) {
      scales[i]       = sin( delta_t_s +  90.0f * count );
      positions[i].x  = cos( delta_t_s + 120.0f * count );
      positions[i].y += .1 * delta_t_s;
      positions[i].z  = sin( delta_t_s +  45.0f * count );
   }
}

class ParticleSystem {

ParticleSystem( TexD, TexS, TexN, TexE, lambda ) {}


template <class T_ParticleLambda>
void set_behaviour( T_ParticleLambda lambda ) {

   auto print = []( String x ) { std::cout << x; };

std::function<void(String)> print = []( auto x ) { std::cout << x; };
std::invocable


   print( "Hej");
   print( Float32(1.2345f) );

   call_on_elements( print, vector );

}


void set_behaviour( std::function<void(Uint32&,Vec3*,Float32*,Float32*,Float32 const)> lambda ) {

}

void update( Float32 delta_t_s ) {
   lambda( _count, _positions, _scales, _opacities, delta_t_s );
}


// kommer att kallas mellan geometry och lighting passes
void draw() {
   // bind alla uniformerna, textuerna, osv
   // kör shaderprogrammet
}

};


