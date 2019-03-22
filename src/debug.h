#pragma once

#include "misc/defs.h"
#include "misc/ImGui/imgui.h"
#include "Config.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "Viewport.h"

namespace debug {
   void view_mat4( Mat4 const &, String name ); // name also serves as window title & ID
   void lightsource( SharedPtr<Light>, SceneManager & );
}; // namespace end


class Viewport;
class AssetManager;
class SceneManager;


class DebugGUI {
public:

   void draw() {
      ImGui::BeginMainMenuBar();
         if ( ImGui::BeginMenu("Main") ) {
            ImGui::MenuItem("New Scene" );
            ImGui::MenuItem("Load Scene" );
            ImGui::MenuItem("Save Scene" );
            ImGui::Separator();
            if ( ImGui::MenuItem("Options" ) )
               _is_options_win_shown = true;
            ImGui::Separator();
            ImGui::MenuItem("Quit" );
            ImGui::EndMenu();
         }
         if ( ImGui::BeginMenu("Scene") ) {
            ImGui::MenuItem( "Load Model" );
            ImGui::Separator();
            if ( ImGui::MenuItem( "Model Instances"  ) )
               _is_models_win_shown = true;
            if ( ImGui::MenuItem( "Light Instances"  ) )
               _is_lights_win_shown = true;
            if ( ImGui::MenuItem( "Particle Systems" ) )
               _is_particle_win_shown = true;
            // TODO: textures
            ImGui::EndMenu();
         }
         if ( ImGui::BeginMenu("Info") ) {
            if ( ImGui::MenuItem( "User Guide" ) )
               _is_startup_message_shown = true;
            ImGui::Separator();
            ImGui::MenuItem( "Debug Log"  );
            ImGui::EndMenu();
         }
      ImGui::EndMainMenuBar();

      _show_startup_message();
      _show_options_window();
      _show_model_instance_window();
      _show_light_instance_window();
      _show_particle_system_window();
   }

private:

   void _show_startup_message() {
      static constexpr int  my_width  = 400;
      static constexpr int  my_height = 320;

      if ( !_is_startup_message_shown )
            return; // end our instructions window early

      ImGui::Begin( "Welcome!", // begin our instructions window:
                    &_is_startup_message_shown,
                     ImGuiWindowFlags_NoCollapse   |
                     ImGuiWindowFlags_NoResize     |
                     ImGuiWindowFlags_NoMove       |
                     ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoNavInputs  );
      {
         // draw our window GUI components and do I/O:
         ImGui::SetWindowPos(  "Welcome", ImVec2( _camera->width/2-my_width/2, _camera->height/2-my_height/2 ) );
         ImGui::SetWindowSize( "Welcome", ImVec2( my_width, my_height ) );
         ImGui::Text( "Instructions:\n \
                     \n\t Press F1    to toggle fly mode               \
                     \n\t Press F2    to toggle wireframe              \
                     \n\t Press F3    to set render mode: SHADED       \
                     \n\t Press F4    to set render mode: ALBEDO       \
                     \n\t Press F5    to set render mode: NORMAL       \
                     \n\t Press F6    to set render mode: SPECULAR     \
                     \n\t Press F7    to set render mode: POSITION     \
                     \n\t Press F8    to set render mode: EMISSION     \
                     \n\t Press F9    to set render mode: LIGHTING     \
                     \n\t Press F10   to set render mode: MOUSEPICK    \
                     \n\t Press F11   to set render mode: DISPLACEMENT \
                     \n\t Press F12   to toggle debug GUI              \
                     \n\t Press P     to initiate dance party          \
                     \n\t Press WASD  to move forward/left/back/right  \
                     \n\t Press CTRL  to descend                       \
                     \n\t Press SPACE to ascend                        \
                     \n\t Hold middle mouse button to look around      \
                     \n\t                                              \
                     \n\t Press ESC to exit the demo." );
      } ImGui::End(); // end our instructions window
   }


   void _show_options_window() {
      if ( !_is_options_win_shown ) return;

      ImGui::NewFrame();
      if (ImGui::TreeNode("Style")) {
         //ImGui::ShowStyleSelector("Temp name");
         ImGui::Separator();
         //ImGui::ShowStyleEditor();
         ImGui::TreePop();
         ImGui::Separator();
      }
      // TODO: graphics etc
      ImGui::EndFrame();
   }

   void _show_model_instance_window() {
      if ( !_is_models_win_shown ) return;
      /*
      ImGui::Begin("Model Instances");
      auto list_copy = _scene_man->_instances;
      std::sort( list_copy.begin(),
                 list_copy.end(),
                 []( auto lhs, auto rhs) {
                    if ( lhs.expired() ) return false;
                    if ( rhs.expired() ) return true;
                    if ( lhs.lock()->id <= rhs.lock()->id ) return true;
                    else return false;
                 } );
      for ( auto &e : list_copy ) {

      }
      ImGui::End();*/
   }

   void _show_light_instance_window() {
      if ( !_is_lights_win_shown ) return;

      ImGui::Begin("Light Instances");
      //auto list_copy = _scene_man._lights;;
      //std::sort( list_copy.begin(),
      //           list_copy.end(),
      //           []( auto lhs, auto rhs) {
      //            return false;
      //              //if ( lhs.expired() ) return false;
      //              //if ( rhs.expired() ) return true;
      //              //if ( lhs.lock()->id <= rhs.lock()->id ) return true;
      //              //else return false;
      //           } );
      //for ( auto &e : list_copy ) {
//
//      //}
      ImGui::End();
   }

   void _show_particle_system_window() {
      if ( !_is_particle_win_shown ) return;

      ImGui::Begin("Particle Systems");
      //auto list_copy = _scene_man._lights;;
      //std::sort( list_copy.begin(),
      //           list_copy.end(),
      //           []( auto lhs, auto rhs) {
      //            return false;
      //              //if ( lhs.expired() ) return false;
      //              //if ( rhs.expired() ) return true;
      //              //if ( lhs.lock()->id <= rhs.lock()->id ) return true;
      //              //else return false;
      //           } );
      //for ( auto &e : list_copy ) {
      //
      //      //}
      ImGui::End();
   }

   SceneManager   *_scene_man;
   AssetManager   *_asset_man;
   Viewport       *_camera; // NOTE: potential disjunction in lifetimes

   Bool _is_startup_message_shown = false,
        _is_options_win_shown     = false,
        _is_models_win_shown      = false,
        _is_lights_win_shown      = false,
        _is_particle_win_shown    = false;

public:
      DebugGUI( SceneManager   *scene_man,
                AssetManager   *asset_man,
                Viewport       *camera )
   :
      _scene_man ( scene_man ),
      _asset_man ( asset_man ),
      _camera    ( camera    )
   {
      (void) _asset_man; // temp
      (void) _scene_man; // temp
   }
};

//#ifdef DEBUG
//   SharedPtr<ShaderProgram>                  _debug_line_shader;
//   Vector<WeakPtr<ModelInstance>>            _debug_instances;
//#endif /*DEBUG*/
//
//   //DepthMap stuff for Shadowmapping
//   Uint32                                    _depth_map_FBO_id;
//   HashMap<SharedPtr<Shadowcaster>, Uint32>  _shadow_maps;
//
//   Vector<WeakPtr<ModelInstance>>            _instances;
//
//   Vector<WeakPtr<ParticleSystem>>           _particle_systems;
//
//   HashMap<Uint64, WeakPtr<Light>>           _lights;
//   Array<Light::Data, light_capacity>        _light_data;
//   Array<      Uint64,light_capacity>        _id_of_light_at; // used to ensure the correct removal of lights
//   Uint32                                    _num_lights;
//
//
