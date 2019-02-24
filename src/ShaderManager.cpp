//#pragma once

#include "ShaderManager.h"


// TODO: embed type into filename? extract from within file?
SharedPtr<Shader> ShaderManager::load_shader( String const &filename  ) {
   // if shader is currently loaded, create a shared pointer to it
   if ( _shader_is_loaded[filename] && !_loaded_shaders[filename].expired() )
      return _loaded_shaders[filename].lock(); // return the shared pointer made from the weak pointer

   else {
      //------------------------------------FIRST READ THE SHADER FROM FILE---------------------------//
      // declare files we need
      IfStream      shader_file;
      StringStream  shader_stream;
      String        shader_code;

      Shader::Type type = _extract_type( filename );

      // ensure ifstream objects can throw exceptions:
      shader_file.exceptions( std::ifstream::failbit | std::ifstream::badbit );

      try {
         shader_file.open( Config::shader_path + filename ); // open file
         shader_stream << shader_file.rdbuf();               // read content into stream
         shader_file.close();                                // close file handle
         shader_code = shader_stream.str();                  // convert to string
      }
      catch ( std::ifstream::failure e ) {
         std::cout << "[ERROR] Unable to read shader code from file:\n\t" << e.what();
         //assert( false && "[ERROR] Unable to read shader code from file." ); // temp
      }

      //-------------------------------------THEN COMPILE THE SHADER----------------------------------//
      // create a shared pointer to the shader
      auto shader_ptr = std::make_shared<Shader>( shader_code.c_str(), type );
      // add a weak version to the loaded shaders list
      _shader_is_loaded[filename] = true; // TODO: (low prio) will never reset--not scalable!
      _loaded_shaders[filename]   = WeakPtr<Shader>( shader_ptr );

      //----------------------------------------THEN RETURN-------------------------------------------//
      return shader_ptr;
   }
}

SharedPtr<ShaderProgram> ShaderManager::create_program( Vector<SharedPtr<Shader>> shaders) {
   return std::make_shared<ShaderProgram>( shaders );
}

Shader::Type ShaderManager::_extract_type( StringView filename ) const {
      auto extension = filename.substr( filename.find_last_of(".") + 1 ) ;
      if      ( extension == "vert" )
          return Shader::Type::vertex;
      else if ( extension == "geom" )
          return Shader::Type::geometry;
      else if ( extension == "frag" )
          return Shader::Type::fragment;
      else {
         assert(false && "Unaccounted for Shader-extension");
         return Shader::Type::error;
      }
  }
