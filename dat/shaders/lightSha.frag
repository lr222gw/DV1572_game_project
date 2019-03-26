#version 440 core

uniform sampler2D   shadowMap;
uniform mat4        lightmatrix;

in       vec2       uv_fs;
uniform  sampler2D  g_tex_pos;
uniform  sampler2D  g_tex_norm;
uniform  sampler2D  g_tex_spec;
uniform  sampler2D  g_tex_albedo;
uniform  sampler2D  g_tex_emit;
uniform  sampler2D  g_tex_disp;
uniform  sampler2D  g_tex_pic;
out      vec4       rgba_rasterizer;

#define AMBIENT_FACTOR 0.18f
#define DIFFUSE_FACTOR 1.0f - 0.18f

struct Light {
  uint  type;
  vec3  dir;
  vec3  pos;
  vec3  rgb;
  float intensity;
  float radius;
  float degree;
  float specularity;
};

// light types "enum" (needs to match Light::Type)
const uint        point_light_t = 0,
                   spot_light_t = 1,
            directional_light_t = 2;

// render mode "enum" (needs to match Config::RenderMode)
const uint mode_composite    = 0,
           mode_albedo       = 1,
           mode_specular     = 2,
           mode_normals      = 3,
           mode_emissive     = 4,
           mode_displacement = 5,
           mode_positional   = 6,
           mode_textureless  = 7,
           mode_picking	     = 8;

const int lights_cap = 32;

// TODO: take one array of each light type and have a loop for each instead?
uniform Light lights[lights_cap];
uniform uint  num_lights;
uniform uint  render_mode;
uniform vec3  view_pos;

void main() {
   // retrieve data from gbuffer
   vec3  pos      = texture( g_tex_pos,    uv_fs ).rgb;
   vec3  norm     = texture( g_tex_norm,   uv_fs ).rgb;
   vec3  albedo   = texture( g_tex_albedo, uv_fs ).rgb;
   vec3  spec_rgb = texture( g_tex_spec,   uv_fs ).rgb; // TODO: check texture channels
   float spec_str = texture( g_tex_spec,   uv_fs ).w;   // TODO: check texture channels
   vec4  emissive = texture( g_tex_emit,   uv_fs );
   vec3  emit_rgb = emissive.xyz;
   vec3  disp_rgb = texture( g_tex_disp,   uv_fs ).rgb;
   vec4  picking  = texture( g_tex_pic,    uv_fs );

   vec3 view_dir  = normalize( view_pos - pos );

   vec3 lighting;

   switch ( render_mode ) {
      case mode_albedo:        lighting = albedo;    break;
      case mode_normals:       lighting = norm;      break;
      case mode_specular:      lighting = spec_rgb;  break;
      case mode_positional:    lighting = pos;       break;
      case mode_emissive:      lighting = emit_rgb;  break;
      case mode_displacement:  lighting = disp_rgb;  break;
      case mode_picking:       lighting = vec3( min( 255, picking.rgb.x*2 ),   // this is just visual;
                                                min( 255, picking.rgb.y*2 ),   // it doesn't affect the picking G-buffer color attachment
                                                min( 255, picking.rgb.z*2 ) ); break;
      case mode_textureless: if ( pos.x+pos.y+pos.z!=0 ) albedo = vec3( 1.0 ); // no break so that the mode_composite code gets run
      case mode_composite:
         lighting = emit_rgb;
         for ( int i = 0;  i < num_lights;  ++i ) {
           Light light = lights[i];

           if ( light.type == point_light_t ) { // TODO: take one array of each light type and have a loop for each instead
                 float radius   = light.radius;
                 float dist     = length( light.pos - pos );
              if ( dist < radius ) {  //  Blinn-Phong for spec
                 vec3  light_dir        = normalize( light.pos - pos );
                 vec3  halfway_dir      = normalize( light_dir + view_dir );

  			         //light.rgb *= length(light.rgb), light.intensity );
                 // calculate light effect falloff:
                 float dist_effect      = min( max(0.0f, abs(dist)), radius);
                 float linear_falloff   = (radius-dist_effect) / radius;   // yields a normalized value (within the range [0, 1.0])
                 float quad_falloff     = linear_falloff * linear_falloff; // quadratic falloff = linear falloff squared
                 // calculate ambient impact:
                 vec3  ambient_impact   = light.rgb * vec3( AMBIENT_FACTOR );
                 // calculate light diffuse impact:
                 float light_modulation = max( dot(norm,light_dir), 0.0f ); // yields a normalized value (within the range [0, 1.0])
                 vec3  diffuse_impact   = vec3( light_modulation * DIFFUSE_FACTOR)  * light.rgb;
                 // calculate specular impact:
                 float spec_modulation  = pow( max( dot(norm, halfway_dir), 0.0f ), 32.0); // yields a normalized value (within the range [0, 1.0])
                 vec3  spec_impact      = light.rgb * (spec_modulation); // TODO: keep it without spec strength?
                 // update lighting:
                 lighting              += quad_falloff * (spec_impact + ambient_impact + diffuse_impact) * albedo * light.intensity;
                 // TODO: HDR output?
              }
           }
           else if ( light.type == spot_light_t ) {
              lighting = vec3(1.0, 0.0, 1.0 ); // TODO
           }
           else if ( light.type == directional_light_t ) {

		     	//We store a target positione in our directional lights "dir" member. 
				//we can get the dir by creating a vector from the light to whatever it's targeting and normalize it
				light.dir = normalize(light.pos - light.dir) ;

      			// ambient light from lightsource
      			vec3 ambient_impact = light.rgb * vec3(0.2);
      			// diffuse factor, gets multiplied with the light's color
      			float light_modulation   = max(dot(light.dir, norm), 0.0);
      			vec3 diffuse_impact = light_modulation * light.rgb;
      			
				// Specular lighting, this is for regular phong (we use BLINNPHONG for pointlights)
				// Blinnphong gives better result, 
				vec3 reflection = reflect(-light.dir, norm);  
				float spec_modulation  = pow( max( dot(view_dir, reflection), 0.0 ), 128.0); // yields a normalized value (within the range [0, 1.0])
                vec3  spec_impact      = light.rgb * (spec_modulation * spec_str); // TODO: remove spec_str
      			
      			// calculate shadow:
				// resolution can be changed in SceneManager::set_shadowcasting()
      			vec4 pos_lightspace = lightmatrix * vec4(pos, 1.0);
      			vec3 sampling_coords    = pos_lightspace.xyz / pos_lightspace.w; // screen space
      			// transform from [-1,1] to [0,1] range; To sample from our 2D texture (shadowmap)
      			sampling_coords = sampling_coords / 2 + 0.5;
      			// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
      			vec4 shadowmap_depth = texture( shadowMap, sampling_coords.xy );      			
      			// get depth of current fragment from light's perspective
				float shadow_bias    = 0.005; // remove shadow acne      			
      			// check whether current frag pos is in shadow      			
      			float shadow_toggle; // shadow_toggle; 0.0 if shadow, 1.0 if no shadow

				//We check if the current depth from our samping_coords is bigger than the closest
				//If so the current fragment is in shadow; by setting the shadow_toggle to 0.0 it will multiply diffuse and spec with 0 
				//which will result in no added light.(depth from shadow_depth is stored in "x" since the shadow_map only stores a depth value, so the first...)
				if((sampling_coords.z - shadow_bias)  >  shadowmap_depth.x ){ shadow_toggle = 0.0;}else{shadow_toggle = 1.0;}
				//if  we use following code we get the lit zone from our shadowmap, it will be round but also not cast shadows as it should...
					//distance( vec4(0.0), pos_lightspace) < distance(vec4(0.0 ), shadowmap_depth) <-- does not work as intended...

      			lighting += (ambient_impact + shadow_toggle * (diffuse_impact+spec_impact))
                        * albedo
                        * light.intensity;
         }
      }
      break; // end of mode_composite case

      default: lighting = vec3( 1, 0, 1 ); // render pink if erronous mode
   }

   rgba_rasterizer = vec4(lighting, 1.0); // DONE! output fragment color to rasterizer
}

// PCF:
// Percentage Closer Filtering: soften shadow by offsetting in 4 directions (read more on learnOpenGL)
