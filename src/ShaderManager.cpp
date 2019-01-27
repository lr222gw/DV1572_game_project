#pragma once

#include "ShaderManager.h"


// TODO: embed type into filename? extract from within file?
SharedPtr<Shader> ShaderManager::load_shader( String const &filename  ) {
   // if shader is currently loaded, create a shared pointer to it
   if ( _shader_is_loaded[filename] && !_loaded_shaders[filename].expired() )
      return _loaded_shaders[filename].lock(); // return the shared pointer made from the weak pointer

   else {
      //------------------------------------FIRST READ THE SHADER FROM FILE---------------------------//
      // declare files we need
      Ifstream      shader_file;
      StringStream  shader_stream;

      // .vs = vertex   shader
      // .gs = geometry shader
      // .fs = fragment shader
      Shader::Type type = _extract_type( filename );

      // ensure ifstream objects can throw exceptions:
      shader_file.exceptions( std::ifstream::failbit | std::ifstream::badbit );

      try {
         shader_file.open( g_config.shader_path + filename ); // open file
         shader_stream << shader_file.rdbuf();                // read content into stream
         shader_file.close();                                 // close file handle
          const char *shader_code = shader_stream.str();      // convert to string
      }
      catch ( std::ifstream::failure e ) {
         std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
      }

      //-------------------------------------THEN COMPILE THE SHADER----------------------------------//
      // create a shared pointer to the shader
      auto shader_ptr = std::make_shared<Shader>( shader_code, type );
      // add a weak version to the loaded shaders list
      _shader_is_loaded[filename] = true; // TODO: (låg prio) kommer aldrig att bli falsk; ej skalbart dynamiskt
      _loaded_shaders[filename]   = WeakPtr( shader_ptr );

      //----------------------------------------THEN RETURN-------------------------------------------//
      return shader_ptr;

      /* gammal kod med index:
      // om filen laddats in utan problem:
      ShaderId id   = _generate_shader_id( type ); // generate unique Id
      _shaders[id]  = shader_loc;
      return id;
      */
   }
}

SharedPtr<ShaderProgram> ShaderManager::create_program( Vector<SharedPtr<Shader>> shaders) {
   return std::make_shared<ShaderProgram>( shaders );
}

/*
[[nodiscard]] Shader const & ShaderManager::get_shader( ShaderId id ) const {
   if ( _shaders.contains(id) )
      return _shaders[id];
   else throw {}; // TODO: exceptions
}
*/

// extracts the type of a shader from a shader Id
// by extracting the 8 leftmost bits and converting to Shader::Type enum type.
/*
[[nodiscard]]  Shader::Type ShaderManager::get_type( ShaderId id ) const {
    return static_cast<Shader::Type>(id >> 56); // TODO: if it doesn't work, use dynamic_cast
}
*/

  // Maintains a static Id counter that determines the next Id.
  // Embeds the shader type category into the Id by masking the 8 leftmost bits.
   /*
  [[nodiscard]]  ShaderId ShaderManager::_generate_shader_id( Shader::Type type ) {
      static ShaderId next_id { 1 };
      return (type << 56) & next_id++;
  }
  */

  /*
  [[nodiscard]]  ShaderProgramId ShaderManager::_generate_shader_program_id() {
      static ShaderProgramId next_id { 1 };
      return next_id++;
  }
  */
 
 
Shader::Type ShaderManager::_extract_type( StringView filename ) const {
      auto extension = filename.substr( filename.find_last_of(".") + 1) ;
      if      ( extension == "vs" )
          return Shader::Type::vertex;
      else if ( extension == "gs" )
          return Shader::Type::geometry;
      else if ( extension == "fs" )
          return Shader::Type::fragment;
      else assert(false && "Unaccounted for Shader-extension"); // TODO: exceptions
  }
 


