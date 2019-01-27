#pragma once

class ShaderHandler {
public:
   using ShaderId        = Uint32; // 32-bit representation used to representate shaders with unique Ids.
   using ShaderProgramId = Uint32; // 32-bit representation used to representate shader programs with unique Ids.

// TODO: embed type into filename? extract from within file?
[[nodiscard]] SharedPtr<Shader> load_shader( StringView filename  ) {
   // if shader is currently loaded, create a shared pointer to it
   if ( _loaded_shaders_map.contains(filename) && !_loaded_shaders_map[filename].expired() )
      return _loaded_shaders_map[filename].lock(); // return the shared pointer made from the weak pointer

   else {
      //------------------------------------FIRST READ THE SHADER FROM FILE---------------------------//
      // declare files we need
      String        shader_code;
      Ifstream      shader_file;
      StringStream  shader_stream;

      // .vs = vertex   shader
      // .gs = geometry shader
      // .fs = fragment shader
      ShaderType type = _extract_type( filename );

      // ensure ifstream objects can throw exceptions:
      shader_file.exceptions( std::ifstream::failbit | std::ifstream::badbit );

      try {
         shader_file.open( g_config.shader_path + filename ); // open file
         shader_stream << shader_file.rdbuf();                // read content into stream
         shader_file.close();                                 // close file handle
         shader_code = shader_stream.str();                   // convert to string
      }
      catch ( std::ifstream::failure e ) {
         std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
      }
      const char *shader_code_str = shader_stream.c_str();

      //-------------------------------------THEN COMPILE THE SHADER----------------------------------//
      // create a shared pointer to the shader
      auto shader_ptr = std::make_shared<Shader>( shader_code_str, type );
      // add a weak version to the loaded shaders list
      _loaded_shaders_map[filename] = WeakPtr( shader_ptr );

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

[[nodiscard]] ShaderProgramId create_program( Vector<ShaderId> shader_ids ) {
   // local buffer to store error strings when compiling.
   char buffer[1024];
   memset( buffer, 0, 1024 );

   GLuint shader_program = GlCreateProgram();

   for ( auto &id : shaders_ids )
      glAttachShader( shader_program, get_shader(id) );

   glLinkProgram( shader_program );

   auto compile_result = GL_FALSE;
   glGetProgramiv( shader_program, GL_LINK_STATUS, &compile_result );
   if ( compile_result == GL_FALSE ) {
      // query information about the compilation (nothing if compilation went fine!)
      memset( buffer, 0, 1024 );
      glGetProgramInfoLog( shader_program, 1024, nullptr, buffer );
      // print to Visual Studio debug console output
      OutputDebugStringA( buffer );
   }

   auto id = _generate_shader_program_id();
   _shader_programs[id] = shader_program;

   /* TODO: hantera detach och delete?

      // regardless of the compilation status we only need the program
      // so detach the shaders:
      glDetachShader( shader_program, vs );
      glDetachShader( shader_program, gs );
      glDetachShader( shader_program, fs );
      // and  delete the shaders:
      glDeleteShader( vs );
      glDeleteShader( gs );
      glDeleteShader( fs );
   */

   return id;
}

/*
[[nodiscard]] Shader const & get_shader( ShaderId id ) const {
   if ( _shaders.contains(id) )
      return _shaders[id];
   else throw {}; // TODO: exceptions
}
*/

// extracts the type of a shader from a shader Id
// by extracting the 8 leftmost bits and converting to ShaderType enum type.
/*
[[nodiscard]]  ShaderType get_type( ShaderId id ) const {
    return static_cast<ShaderType>(id >> 56); // TODO: if it doesn't work, use dynamic_cast
}
*/

private:
  // Maintains a static Id counter that determines the next Id.
  // Embeds the shader type category into the Id by masking the 8 leftmost bits.
   /*
  [[nodiscard]]  ShaderId _generate_shader_id( ShaderType type ) {
      static ShaderId next_id { 1 };
      return (type << 56) & next_id++;
  }
  */

  /*
  [[nodiscard]]  ShaderProgramId _generate_shader_program_id() {
      static ShaderProgramId next_id { 1 };
      return next_id++;
  }
  */
 
  /* 
  [[nodiscard]] ShaderType _extract_type( StringView filename ) const {
      auto extension = filename.substr( filename.find_last_of(".") + 1) ;
      if      ( extension == "vs" )
          return ShaderType::vertex;
      else if ( extension == "gs" )
          return ShaderType::geometry;
      else if ( extension == "fs" )
          return ShaderType::fragment;
      else throw {}; // TODO: exceptions
  }
  */
   
  /*
  UnorderedMap<ShaderProgramId,GLuint>  _shader_programs; // maps unique shader Ids to loaded shaders. 
  UnorderedMap<ShaderId,GLuint>         _shaders;         // maps unique shader Ids to loaded shaders. 
  */
  UnorderedMap<String,WeakPtr<Shader>>  _loaded_shaders_map;         // maps filenames to weak pointers

};