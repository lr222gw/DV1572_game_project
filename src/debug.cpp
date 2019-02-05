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
