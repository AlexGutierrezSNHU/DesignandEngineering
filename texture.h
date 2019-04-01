/*TEXTURE LOADER TEMPLATE ADAPTED FROM "Learn OpenGL: Beginner's Guide to 3D Rendering and Game Development with OpenGL and C++" BY Frahaan Hussain*/

#pragma once

// GL Includes
#define GLEW_STATIC
#include <GL/glew.h>
#include <SOIL2.h>

#include <vector>

class TextureLoading
{
public:
    static GLuint LoadTexture( const GLchar *path )
    {
        //Generate texture ID and load texture data
        GLuint textureID;
        glGenTextures( 1, &textureID );
        
        int imageWidth, imageHeight;
        
        unsigned char *image = SOIL_load_image( path, &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB );
        
        // Assign texture to ID
        glBindTexture( GL_TEXTURE_2D, textureID );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
        glGenerateMipmap( GL_TEXTURE_2D );
        
        // Assign Texture Parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glBindTexture( GL_TEXTURE_2D,  0);
        
        SOIL_free_image_data(image);
        
        return textureID;
    }
    
  };