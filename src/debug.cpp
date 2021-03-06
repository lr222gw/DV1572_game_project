#include "debug.h"

#include "misc/ImGui/imgui.h"
#include "misc/ImGui/imgui_impl_glfw.h"
#include "misc/ImGui/imgui_impl_opengl3.h"

void debug::view_mat4( Mat4 const &m, String name ) {
   if ( !config.is_imgui_toggled )
      return;

   String id = std::to_string( (Uint64)(&m) );
   ImGui::PushID( id.c_str() );
   ImGui::Begin( "Analysis:" ); // begin our Rotation window:
   {  // draw our window GUI components and do I/O:
      ImGui::NewLine();
      ImGui::Text( "%s:", name.c_str() );

                         ImGui::Value( " ", m[0][0], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[0][1], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[0][2], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[0][3], "%3.1f" );
      ImGui::NewLine();
                         ImGui::Value( " ", m[1][0], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[1][1], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[1][2], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[1][3], "%3.1f" );
      ImGui::NewLine();
                         ImGui::Value( " ", m[2][0], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[2][1], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[2][2], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[2][3], "%3.1f" );
      ImGui::NewLine();
                         ImGui::Value( " ", m[3][0], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[3][1], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[3][2], "%3.1f" );
      ImGui::SameLine(); ImGui::Value( " ", m[3][3], "%3.1f" );
      ImGui::NewLine();
      ImGui::Separator();
   }
   ImGui::End();
   ImGui::PopID();
}

void debug::lightsource( SharedPtr<Light> light, SceneManager &scene_man ) {
   if ( !config.is_imgui_toggled )
      return;

   ImGui::PushID( "Lightsource_debug" );
   char id_str[64];
   snprintf( id_str, 64, "Lightsource %lu:", light->get_id() );
   ImGui::Begin( id_str );
   {

/*      sun->set_type( Light::Type::directional );
      sun->set_direction( glm::normalize(poss - dirr) );
      sun->set_position( poss );
      // sun->set_color( Vec3(1.0f,  1.0f,   1.0f) );
      sun->set_intensity( intensity );
      sun->set_radius( radius );
      sun->set_degree( degree );
      sun->set_specularity( specularity );
      */

      ImGui::Text( "Position:" );

      ImGui::PushItemWidth( 96.0f );

      auto position = light->get_position();

      ImGui::PushID( "lspx" );
      ImGui::InputFloat( "", &position.x, 1.0f, 0.1f, "%3.1f" );
      ImGui::PopID();

      ImGui::SameLine();
      ImGui::PushID( "lspy" );
      ImGui::InputFloat( "", &position.y, 1.0f, 0.1f, "%3.1f" );
      ImGui::PopID();

      ImGui::SameLine();
      ImGui::PushID( "lspz" );
      ImGui::InputFloat( "", &position.z, 1.0f, 0.1f, "%3.1f" );
      ImGui::PopID();

      if ( position != light->get_position() )
         light->set_position( position );

      ImGui::PopItemWidth();

      ImGui::Separator();

      ImGui::Text( "Direction:" );

      ImGui::PushItemWidth( 96.0f );

      auto direction = light->get_direction();

      ImGui::PushID( "lsrx" );
      ImGui::InputFloat( "", &direction.x, 1.0f, 0.1f, "%3.1f" );
      ImGui::PopID();

      ImGui::SameLine();
      ImGui::PushID( "lsry" );
      ImGui::InputFloat( "", &direction.y, 1.0f, 0.1f, "%3.1f" );
      ImGui::PopID();

      ImGui::SameLine();
      ImGui::PushID( "lsrz" );
      ImGui::InputFloat( "", &direction.z, 1.0f, 0.1f, "%3.1f" );
      ImGui::PopID();

      if ( direction != light->get_direction() )
         light->set_direction( direction );

      //intensity, Float32 &radius, Float32 &degree, Float32 &specularity
      ImGui::PopItemWidth();

      ImGui::Separator();

      ImGui::Text("Light variables:");

      ImGui::PushItemWidth(85.0f);

      auto intensity = light->get_intensity();

      ImGui::PushID("lvrx");
      ImGui::InputFloat(": Intensitet ", &intensity, 0.01f, 0.05f, "%1.2f");
      ImGui::PopID();

      if ( intensity != light->get_intensity() )
         light->set_intensity( intensity );

        //ImGui::PushID("lvry");
        //ImGui::InputFloat(": Radius ", &radius, 1.0f, 0.1f, "%3.1f");
        //ImGui::PopID();
        //
        //
        //ImGui::PushID("lvrz");
        //ImGui::InputFloat(": Degree ", &degree, 1.0f, 0.1f, "%3.1f");
        //ImGui::PopID();

      auto specularity = light->get_specularity();

      ImGui::PushID("lvru");
      ImGui::InputFloat(": Specular ", &specularity, 1.0f, 0.1f, "%3.1f");
      ImGui::PopID();

      if ( specularity != light->get_specularity() )
         light->set_specularity( specularity );

      ImGui::PopItemWidth();
   }
   ImGui::End();
   ImGui::PopID();
}
