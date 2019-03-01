#version 440 core

in VS_OUT {
    vec3 FragPos;
    //vec3 Normal;
    vec2 uv;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;
uniform mat4      lightmatrix;

//in       vec2       uv_fs;
uniform  sampler2D  g_tex_pos;
uniform  sampler2D  g_tex_norm;
uniform  sampler2D  g_tex_spec;
uniform  sampler2D  g_tex_albedo;
uniform  sampler2D  g_tex_emit;
uniform  sampler2D  g_tex_pic;
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

const uint        point_light_t = 0,
                   spot_light_t = 1,
            directional_light_t = 2;

const uint     mode_composite   = 0,
               mode_albedo      = 1,
               mode_normals     = 2,
               mode_specular    = 3,
               mode_positional  = 4,
               mode_emissive    = 5,
               mode_textureless = 6,
               mode_picking	    = 7;

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
   vec4  emissive = texture( g_tex_emit,   fs_in.uv );
   vec3  emit_rgb = emissive.xyz;
   vec4 picking = texture(g_tex_pic, fs_in.uv);

   vec3 view_dir  = normalize( view_pos - pos );

   vec3 lighting;

   switch ( render_mode ) {
      case mode_albedo:      lighting = albedo;      break;
      case mode_normals:     lighting = norm;        break;
      case mode_specular:    lighting = spec_rgb;    break; // TODO: check texture channels
      case mode_positional:  lighting = pos;         break;
      case mode_emissive:    lighting = emit_rgb;    break;
      case mode_picking:     lighting = picking.rgb; break;
      case mode_textureless: if ( pos.x+pos.y+pos.z!=0 ) albedo = vec3( 1.0 ); // no break so that the mode_composite code gets run
      case mode_composite:
         lighting = albedo * 0.2; // start off with ambient light

         for ( int i = 0;  i  < num_lights;  ++i ) {
           Light light = lights[i];
           if ( light.type == point_light_t ) { // TODO: take one array of each light type and have a loop for each instead
              ////////////////////////////////////////////////////////////////////////////////////////
              float radius   = light.radius * 100.0;
              float distance = length( light.pos - pos );
              if ( distance < radius ) {
                 vec3  light_dir        = normalize( light.pos - pos );
                 vec3  halfway_dir      = normalize( light_dir + view_dir );

  			         light.rgb = light.rgb * light.intensity;

                 // calculate light effect falloff:
                 float linear_falloff   = (1.0 - distance / radius);       // yields a normalized value (within the range [0, 1.0])
                 float quad_falloff     = linear_falloff * linear_falloff; // quadratic falloff = linear falloff squared
                 // calculate light diffuse impact:
                 float light_modulation = max( dot(norm, light_dir), 0.0f ); // yields a normalized value (within the range [0, 1.0])
                 vec3  diffuse_impact   = albedo * light.rgb * (light_modulation/* * light.intensity */* quad_falloff);
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
           else if ( light.type == directional_light_t ) { // light.type == directional_light_t
      			vec3 normal = norm;
      			//vec3 lightColor = vec3(0.3);
      			// ambient
      			vec3 ambient = 0.3 * albedo;
      			// diffuse
      			vec3 lightDir = normalize(light.pos - pos);
      			float diff = max(dot(light.dir, normal), 0.0);
      			vec3 diffuse = diff * light.rgb;
      			// specular
      			vec3 viewDir = normalize(view_pos - pos);
      			vec3 reflectDir = reflect(-light.dir, normal);
      			float spec = spec_str;
      			vec3 halfwayDir = normalize(light.dir + viewDir);
      			spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
      			vec3 specular = spec * light.rgb;
      			// calculate shadow

      			vec4 lightSpacePos = lightmatrix * vec4(pos, 1.0f);//fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;

      			vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
      			// transform to [0,1] range
      			projCoords = projCoords * 0.5 + 0.5;
      			// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
      			float closestDepth = texture( shadowMap, projCoords.xy ).r;
      			// get depth of current fragment from light's perspective
      			float currentDepth = projCoords.z;
      			// check whether current frag pos is in shadow
      			float shadowBias = 0.005;
      			float shadow = currentDepth - shadowBias > closestDepth  ? 1.0 : 0.0;

      			//TODO: Disable to not test last light...
      			//lighting += (ambient + (1.0 - shadow) * (diffuse + specular)) * albedo;
      			lighting += ((ambient + (1.0 - shadow) * (diffuse + specular)) * albedo) * light.intensity;

            //rgba_rasterizer = vec4(lighting, 1.0);
            //rgba_rasterizer = vec4(lighting, 1.0);
            //lighting = vec3(1.0, 0.0, 1.0 ); // TODO
         }
      }
      lighting += emit_rgb;
      break; // end of mode_composite case

      default: lighting = vec3( 1, 0, 1 ); // render pink if erronous mode
   }

   rgba_rasterizer = vec4(lighting, 1.0); // DONE! output fragment color to rasterizer
}
