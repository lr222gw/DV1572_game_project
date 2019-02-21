#version 440 core



in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 uv;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;



//in       vec2       uv_fs;
uniform  sampler2D  g_tex_pos;
uniform  sampler2D  g_tex_norm;
uniform  sampler2D  g_tex_spec;
uniform  sampler2D  g_tex_albedo;
out      vec4       rgba_rasterizer;

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

const uint       point_light_t = 0,
                  spot_light_t = 1,
           directional_light_t = 2;

const uint     mode_composite  = 0,
               mode_albedo     = 1,
               mode_normals    = 2,
               mode_specular   = 3,
               mode_positional = 4;
//             mode_emission   = 5;

const int lights_cap = 32;

// TODO: take one array of each light type and have a loop for each instead?
uniform Light lights[lights_cap];
uniform uint  num_lights;
uniform uint  render_mode;
uniform vec3  view_pos;

void main() {
   // retrieve data from gbuffer
   vec3  pos      = texture( g_tex_pos,    fs_in.uv ).rgb;
   vec3  norm     = texture( g_tex_norm,   fs_in.uv ).rgb;
   vec3  albedo   = texture( g_tex_albedo, fs_in.uv ).rgb;
   vec3  spec_rgb = texture( g_tex_spec,   fs_in.uv ).rgb; // TODO: check texture channels
   float spec_str = texture( g_tex_spec,   fs_in.uv ).w;   // TODO: check texture channels
// vec3  emit_rgb = texture( g_tex_emit,   uv_fs).rgb;  // TODO: add emission?

   vec3 view_dir  = normalize( view_pos - pos );

   vec3 lighting;


//   for ( ... ) // för alla point lights

//   for ( ... ) // för alla directional lights


   switch ( render_mode ) {
      case mode_albedo:      lighting = albedo;   break;
      case mode_normals:     lighting = norm;     break;
      case mode_specular:    lighting = spec_rgb; break; // TODO: check texture channels
      case mode_positional:  lighting = pos;      break;
      case mode_composite:
         lighting = albedo * 0.2 + vec3(0.05); // start off with ambient light

         for ( int i = 0;  i  < num_lights;  ++i ) {
         Light light = lights[i];
         if ( light.type == point_light_t ) { // TODO: take one array of each light type and have a loop for each instead
            ////////////////////////////////////////////////////////////////////////////////////////
            float radius   = light.radius * 100.0;
            float distance = length( light.pos - pos );
            if ( distance < radius ) {
               vec3  light_dir        = normalize( light.pos - pos );
               vec3  halfway_dir      = normalize( light_dir + view_dir );
               // calculate light effect falloff:
               float linear_falloff   = (1.0 - distance / radius);       // yields a normalized value (within the range [0, 1.0])
               float quad_falloff     = linear_falloff * linear_falloff; // quadratic falloff = linear falloff squared
               // calculate light diffuse impact:
               float light_modulation = max( dot(norm, light_dir), 0.0f ); // yields a normalized value (within the range [0, 1.0])
               vec3  diffuse_impact   = albedo * light.rgb * (light_modulation * light.intensity * quad_falloff);
               // calculate specular impact:
               float spec_modulation  = max( dot(norm, halfway_dir), 0.0f ); // yields a normalized value (within the range [0, 1.0])
               //vec3  spec_impact      = vec3(0.1)/num_lights;//(spec_modulation * spec_str * quad_falloff) * spec_rgb; // mix( spec_rgb, light.rgb, 0.5f );
               vec3  spec_impact      = light.rgb * (spec_modulation * spec_str * quad_falloff);
               // update lighting:
               lighting              += spec_impact + diffuse_impact; // TODO: emission (+ emit_rgb)
               // TODO: HDR output?
            }





            /*
            vec3  light_dir   = normalize( light.pos - pos );
            float light_dist  = length(    light.pos - pos ) / radius;
            vec3  halfway_dir = normalize( light_dir + view_dir );
            vec3  diffuse     = max( dot(norm, light.dir), 0.0 )
                                * light.rgb
                                * light.intensity
                                * albedo;

            if ( radius > light_dist) {
               float spec_modulation  = pow( max( dot(norm, halfway_dir), 0.0 ), 16.0);
               float light_modulation = max( dot(norm, light_dir), 0.0 );
               vec3  specular         = light.rgb * spec_modulation  * spec_str;
               vec3  phong            = light.rgb * light_modulation * light.intensity;
               float attenuation      = 1.0f;
               //float attenuation      = light.rgb * light_modulation * light.intensity;
               diffuse  *= attenuation; // * light.intensity;
               specular *= attenuation; // * light.specularity;
               phong    *= attenuation;
               lighting += diffuse + specular + phong;
            }
            */
            ////////////////////////////////////////////////////////////////////////////////////////
         }
         else if ( light.type == spot_light_t ) {
            lighting = vec3(1.0, 0.0, 1.0 ); // TODO
         }
         else if (light.type == directional_light_t ){ // light.type == directional_light_t

			
			vec3 normal = normalize(fs_in.Normal);
			//vec3 lightColor = vec3(0.3);
			// ambient
			vec3 ambient = 0.3 * albedo;
			// diffuse
			vec3 lightDir = normalize(light.pos - fs_in.FragPos);
			float diff = max(dot(light.dir, normal), 0.0);
			vec3 diffuse = diff * light.rgb;
			// specular
			vec3 viewDir = normalize(view_pos - fs_in.FragPos);
			vec3 reflectDir = reflect(-light.dir, normal);
			float spec = spec_str;
			vec3 halfwayDir = normalize(light.dir + viewDir);  
			spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
			vec3 specular = spec * light.rgb;    
			// calculate shadow                   

			vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
			// transform to [0,1] range
			projCoords = projCoords * 0.5 + 0.5;
			// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
			float closestDepth = texture(shadowMap, projCoords.xy).r; 
			// get depth of current fragment from light's perspective
			float currentDepth = projCoords.z;
			// check whether current frag pos is in shadow
			float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;


			lighting += (ambient + (1.0 - shadow) * (diffuse + specular)) * albedo;    
			
			//rgba_rasterizer = vec4(lighting, 1.0);
			 break;
			 //rgba_rasterizer = vec4(lighting, 1.0);
            //lighting = vec3(1.0, 0.0, 1.0 ); // TODO
         }
      } break; // end of mode_omposite case

      default: lighting = vec3( 1, 0, 1 ); // render pink if erronous mode
   }

   rgba_rasterizer = vec4(lighting, 1.0); // DONE! output fragment color to rasterizer
}
