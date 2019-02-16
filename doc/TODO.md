
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


