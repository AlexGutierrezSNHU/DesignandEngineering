/*SHADER TEMPLATE ADAPTED FROM "Learn OpenGL: Beginner's Guide to 3D Rendering and Game Development with OpenGL and C++" BY Frahaan Hussain*/

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>

#include <iostream>

#include <GL/glew.h>

class Shader
{
public:
    GLuint Program;
   
   // Builds Shader Program from shader source code file path
    Shader( const GLchar *vertexPath, const GLchar *fragmentPath )
    {
        
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        
		//Handles exception if file path is invalid or can not be opened
        vShaderFile.exceptions ( std::ifstream::badbit );
        fShaderFile.exceptions ( std::ifstream::badbit );
        try
        {
            
			//Open file and read content into input stream
            vShaderFile.open( vertexPath );
            fShaderFile.open( fragmentPath );
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf( );
            fShaderStream << fShaderFile.rdbuf( );
            
			// close file handlers
            vShaderFile.close( );
            fShaderFile.close( );
            
			//Passes file from input stream
            vertexCode = vShaderStream.str( );
            fragmentCode = fShaderStream.str( );
        }
        catch ( std::ifstream::failure e )
        {
            std::cout << "ERROR SHADER FILE NOT AVAILABLE" << std::endl;
        }
        const GLchar *vShaderCode = vertexCode.c_str( );
        const GLchar *fShaderCode = fragmentCode.c_str( );
       
	   // Compiles Vertex Shader
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];
        vertex = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( vertex, 1, &vShaderCode, NULL );
        glCompileShader( vertex );
        
		// Return Compilation Error if Caught
        glGetShaderiv( vertex, GL_COMPILE_STATUS, &success );
        if ( !success )
        {
            glGetShaderInfoLog( vertex, 512, NULL, infoLog );
            std::cout << "ERROR SHADER VERTEX COMPILER FAILED\n" << infoLog << std::endl;
        }
        
		// Compiles Fragment Shader
        fragment = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( fragment, 1, &fShaderCode, NULL );
        glCompileShader( fragment );
        
		// Return Compilation Error if Caught
        glGetShaderiv( fragment, GL_COMPILE_STATUS, &success );
        if ( !success )
        {
            glGetShaderInfoLog( fragment, 512, NULL, infoLog );
            std::cout << "ERROR SHADER FRAGMENT COMPILER FAILED\n" << infoLog << std::endl;
        }
        
		//Shader Program
        this->Program = glCreateProgram( );
        glAttachShader( this->Program, vertex );
        glAttachShader( this->Program, fragment );
        glLinkProgram( this->Program );
        
		//Return error if unable to link  Shader Program
        glGetProgramiv( this->Program, GL_LINK_STATUS, &success );
        if (!success)
        {
            glGetProgramInfoLog( this->Program, 512, NULL, infoLog );
            std::cout << "ERROR SHADER ROGRAM LINKING FAILED\n" << infoLog << std::endl;
        }
        // Delete the shaders and free the memory
        glDeleteShader( vertex );
        glDeleteShader( fragment );
        
    }
    // Uses the current shader
    void Use( )
    {
        glUseProgram( this->Program );
    }
};

#endif