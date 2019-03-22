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
      case mode_picking:       lighting = vec3( min( 255, picking.rgb.x*2 ), // this is just visual;
                                                min( 255, picking.rgb.y*2 ), // it doesn't affect the picking G-buffer color attachment
                                                min( 255, picking.rgb.z*2 ) ); break;
      case mode_textureless: if ( pos.x+pos.y+pos.z!=0 ) albedo = vec3( 1.0 ); // no break so that the mode_composite code gets run
      case mode_composite:
         lighting = vec3(0); //emit_rgb; 0xFF00FF
         for ( int i = 0;  i < num_lights;  ++i ) { // 0xFF00FF
           Light light = lights[i];

           if ( light.type == point_light_t ) { // TODO: take one array of each light type and have a loop for each instead
                 float radius   = light.radius * 4;
                 vec3  dist_v   = light.pos - pos;
                 float dist_f   = sqrt( (dist_v.x*dist_v.x) + (dist_v.y * dist_v.y) + (dist_v.z * dist_v.z) );
              // float radius   = light.radius * 4; // 0xFF00FF
              // float dist     = length( light.pos - pos );
              if ( dist_f < radius ) { //  Blinn-Phong for spec
                 vec3  lite_dir = normalize( light.pos - pos );
                 vec3  normal   = normalize( norm );
                 float diff_mod = 0.5f * dot(normal,lite_dir) + 0.5f;
                 lighting += diff_mod * vec3( 1, 0, 1 );
                /*
                 vec3  light_dir        = normalize( light.pos - pos );
                 vec3  halfway_dir      = normalize( light_dir + view_dir );

                 vec3 normal = normalize(norm);

  			         light.rgb = light.rgb; //* light.intensity * 20; // UGLY // 0xFF00FF
                 // calculate light effect falloff:
                 //float linear_falloff   = max( (radius-dist)/radius, 0.0f );         // yields a normalized value (within the range [0, 1.0])
                 //float quad_falloff     = linear_falloff * linear_falloff;     // quadratic falloff = linear falloff squared
                 // calculate ambient impact:
                 //vec3  ambient_impact   = light.rgb * vec3( AMBIENT_FACTOR );
                 // calculate light diffuse impact:
                 float light_modulation = max( dot(normal,light_dir), 0.0f ); //0.5f * dot(normal,light_dir) + 0.5f;   // yields a normalized value (within the range [0, 1.0])
                 vec3  diffuse_impact   = light_modulation * light.rgb; //(light_modulation * light.rgb );
                 // calculate specular impact:
                 // float spec_modulation  = pow( max( dot(normal, halfway_dir), 0.0f ), 32.0); // yields a normalized value (within the range [0, 1.0])
                 // vec3  spec_impact      = light.rgb * (spec_modulation * spec_str); // TODO: remove spec_str
                 // update lighting:
                 lighting              += diffuse_impact * albedo; //(ambient_impact + spec_impact + diffuse_impact) * quad_falloff * albedo ; // TODO: emission (+ emit_rgb) // 0xFF00FF
                 // TODO: HDR output?
                 //*/
              }
           }
           else if ( light.type == spot_light_t ) {
              lighting = vec3(1.0, 0.0, 1.0 ); // TODO
           }
           else if ( light.type == directional_light_t ) {

		     		light.dir = normalize(light.pos - light.dir ) ;
      			// ambient
      			vec3 ambient = light.rgb * vec3(0.2);
      			// diffuse
      			float diff   = max(dot(light.dir, norm), 0.0);
      			vec3 diffuse = diff * light.rgb;
      			// specular
      			vec3  viewDir    = normalize(view_pos - pos);
      			vec3  reflectDir = reflect(-light.dir, norm);
      			float spec       = spec_str;
      			vec3  halfwayDir = normalize(light.dir + viewDir);
      			spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
      			vec3 specular = spec * light.rgb;
      	// calculate shadow:
            // resolution can be changed in SceneManager::set_shadowcasting()
      			vec4 lightSpacePos = lightmatrix * vec4(pos, 1.0f);
      			vec3 projCoords    = lightSpacePos.xyz / lightSpacePos.w; // screen space
      			// transform from [-1,1] to [0,1] range
      			projCoords = projCoords / 2 + 0.5f;
      			// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
      			float closestDepth = texture( shadowMap, projCoords.xy ).r;
      			// get depth of current fragment from light's perspective
      			float currentDepth = projCoords.z;
      			// check whether current frag pos is in shadow
      			float shadowBias    = 0.005; // remove shadow acne
      			float shadow_toggle = currentDepth - shadowBias > closestDepth ? 0.0 : 1.0; // disables light
      			lighting += (ambient + shadow_toggle * (diffuse+specular))
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
