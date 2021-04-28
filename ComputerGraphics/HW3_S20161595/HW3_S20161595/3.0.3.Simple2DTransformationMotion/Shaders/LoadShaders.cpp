#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include "LoadShaders.h"

//----------------------------------------------------------------------------

GLchar* ReadShader( const char* filename ) {

    FILE* infile = fopen( filename, "rb" );

    if ( !infile ) {
#ifdef _DEBUG
		fprintf(stdout, "Unable to open file '%s'\n", filename);
#endif /* DEBUG */
        return NULL;
    }

    fseek( infile, 0, SEEK_END );
    int len = ftell( infile );
    fseek( infile, 0, SEEK_SET );

	GLchar* source = (GLchar*)malloc((len + 1) * sizeof(GLchar));

    fread( source, 1, len, infile );
    fclose( infile );

    source[len] = 0;

    return source;
}

//----------------------------------------------------------------------------

GLuint LoadShaders( ShaderInfo* shaders ) {
    if ( shaders == NULL ) { return 0; }

    GLuint program = glCreateProgram();

    ShaderInfo* entry = shaders;
    while ( entry->type != GL_NONE ) {
        GLuint shader = glCreateShader( entry->type );

        entry->shader = shader;

        GLchar* source = ReadShader( entry->filename );
        if ( source == NULL ) {
            for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
                glDeleteShader( entry->shader );
                entry->shader = 0;
            }

            return 0;
        }

        glShaderSource( shader, 1, &source, NULL );
        free(source);
	
        glCompileShader( shader );

        GLint compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
#ifdef _DEBUG
            GLsizei len;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

			GLchar* log = (GLchar*)malloc((len + 1) * sizeof(GLchar));
            glGetShaderInfoLog( shader, len, &len, log );
			fprintf(stdout, "Shader compilation failed: %s\n", log);
			free(log);
#endif /* DEBUG */

            return 0;
        }

        glAttachShader( program, shader );
        
        ++entry;
    }
    
    glLinkProgram( program );

    GLint linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
#ifdef _DEBUG
        GLsizei len;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

		GLchar* log = (GLchar*)malloc((len + 1) * sizeof(GLchar));
        glGetProgramInfoLog( program, len, &len, log );
		fprintf(stdout, "Shader linking failed: %s\n", log);
		free(log);
#endif /* DEBUG */

        for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
            glDeleteShader( entry->shader );
            entry->shader = 0;
        }
        
        return 0;
    }

    return program;
}


