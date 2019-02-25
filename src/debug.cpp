#include "debug.h"

#include "misc/ImGui/imgui.h"
#include "misc/ImGui/imgui_impl_glfw.h"
#include "misc/ImGui/imgui_impl_opengl3.h"

void debug::view_mat4( Mat4 const &m, String name ) {
   String id = std::to_string( (Uint64)(&m) );
   ImGui::PushID( id.c_str() );
   ImGui::Begin( "Analysis:" ); // begin our Rotation window:
   {  // draw our window GUI components and do I/O:
      ImGui::Text( "%s:", name.c_str() );
      ImGui::Value( "", m[0][0], "%3.1f");
      ImGui::Value( "", m[0][1], "%3.1f");
      ImGui::Value( "", m[0][2], "%3.1f");
      ImGui::Value( "", m[0][3], "%3.1f");
      ImGui::NewLine();
      ImGui::Value( "", m[1][0], "%3.1f");
      ImGui::Value( "", m[1][1], "%3.1f");
      ImGui::Value( "", m[1][2], "%3.1f");
      ImGui::Value( "", m[1][3], "%3.1f");
      ImGui::NewLine();
      ImGui::Value( "", m[2][0], "%3.1f");
      ImGui::Value( "", m[2][1], "%3.1f");
      ImGui::Value( "", m[2][2], "%3.1f");
      ImGui::Value( "", m[2][3], "%3.1f");
      ImGui::NewLine();
      ImGui::Value( "", m[3][0], "%3.1f");
      ImGui::Value( "", m[3][1], "%3.1f");
      ImGui::Value( "", m[3][2], "%3.1f");
      ImGui::Value( "", m[3][3], "%3.1f");
   }
   ImGui::End();
   ImGui::PopID();
}

void debug::lightsource( Vec3 &position, Vec3 &direction, SceneManager &scene_man ) {
   ImGui::PushID( "Lightsource_debug" );
   ImGui::Begin( "Lightsource:" );
   {
      ImGui::Text( "Position:" );

      ImGui::PushItemWidth( 96.0f );

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

      ImGui::PopItemWidth();

      ImGui::Separator();

      ImGui::Text( "Direction:" );

      ImGui::PushItemWidth( 96.0f );

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

      ImGui::PopItemWidth();
   }
   ImGui::End();
   ImGui::PopID();
}
