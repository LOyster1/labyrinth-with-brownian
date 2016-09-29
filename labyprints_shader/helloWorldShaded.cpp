//
// A GLSL "Hellow World Smoothly Shaded" 
// Display a Smoothly Shaded Quad by adding color attributes 
//
// We use the default orthographic projection
//
//GLSL version 430
//


#include "Angel.h"
void m_glewInitAndVersion(void);

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

//Mesh 0
GLuint vao;
const int NumVertices = 4;

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 points[4] = {
    point4( 0.25, 0.25,  0.0, 1.0 ),
    point4( 0.75, 0.25,  0.0, 1.0 ),
    point4( 0.75, 0.75,  0.0, 1.0 ),
    point4( 0.25, 0.75,  0.0, 1.0 )
};


// RGBA colors
color4 colors[4] = {
    color4( 1.0, 0.0,  0.0, 1.0 ),
    color4( 0.0, 1.0,  0.0, 1.0 ),
    color4( 0.0, 0.0,  1.0, 1.0 ),  
	color4( 0.0, 1.0,  0.0, 1.0 )
};

//uniform variable locations
GLuint model_view_loc;

mat4 ctmat = Angel::mat4(1.0);



//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	// Create a vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
		
    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
	
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
	//Allocate space on the server (the gpu device) for the vertex attributes 
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors), NULL, GL_STATIC_DRAW );

	//Load the arrays of data (they are copied the GPU) 
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

   // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader01_v430.glsl", "fshader01_v430.glsl" );
    glUseProgram( program );

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	
	GLuint vColor = glGetAttribLocation( program, "vColor" );
	glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)) );

	model_view_loc = glGetUniformLocation(program, "modelview");

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );


	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, ctmat);
	glBindVertexArray(vao);
	glDrawArrays( GL_QUADS, 0, NumVertices );

	glFlush();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033:  // Escape key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    }
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowSize( 512, 512 );

    glutCreateWindow( "Hello World with Shading" );
	m_glewInitAndVersion();
	

    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    glutMainLoop();
    return 0;
}

void m_glewInitAndVersion(void)
{
   fprintf(stdout, "OpenGL Version: %s\n", glGetString(GL_VERSION));
   fprintf(stdout, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
   GLenum err = glewInit();
   if (GLEW_OK != err)
   {
   fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
   }
   fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}