//G. Kamberov
// A GLSL "Small Labirynth" 
// The program opens two windows showing the labyrinth and a 
// scaled version version of it in separate windows
// The goal is to illustrate: 
//     Basic handling of multiple objects
//     Basic windows handling using freeGLUT (or GLUT)
//     The need to think about proper adjustment of the MVT 
//     
//
// A GLSL "Small Labirynth" 
// On a 3 by 3 grid
//
// We use the default orthographic projection
//
//GLSL version 110

#include "Angel.h"

GLfloat t1 = 1.0/3.0 - 0.5;
GLfloat t2 = 2.0/3.0 - 0.5;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLuint vao;

//

//two walls: one has 1 segment the other has 7 segments
//wall segment 0
point4 points0[2] = {
	point4(-0.5, -0.5,  0.0, 1.0),
	point4(0.5, -0.5,  0.0, 1.0)
};

//wall segment 1
point4 points1[8] = {
	point4(t2,  t1, 0.0, 1.0),
	point4(-0.5,  t1, 0.0, 1.0),
	point4(-0.5, 0.5, 0.0, 1.0),
	point4(0.5, 0.5, 0.0, 1.0),
	point4(0.5,  t2, 0.0, 1.0),
	point4(t1,  t2, 0.0, 1.0),
	point4(0.5,  t2, 0.0, 1.0),
	point4(0.5,  t1, 0.0, 1.0)
};


// RGBA colors
color4 blue_opaque = color4( 0.0, 0.0, 1.0, 1.0 );

//uniform variable locations
GLuint color_loc;
GLuint model_view_loc;
mat4 ctmat = Angel::mat4(1.0);

GLfloat xLast, yLast;
GLfloat displacement;



//----------------------------------------------------------------------------
#include "hwk1.h"

void randomDisplacement(GLfloat magnitude, GLfloat &side1, GLfloat &side2)
{
	GLfloat angle = ((GLfloat)rand() / (GLfloat)RAND_MAX) * (2 * PI);
	side1 = magnitude * cos(angle);
	side2 = magnitude * sin(angle);
}

//this will count all the nodes after head;
int pointCount(struct pointNode* head)
{
	pointNode* tmp;
	tmp = head;
	int count_l = 0;

	while (tmp != NULL)
	{
		count_l++;
		tmp = tmp->next;
	}
	return count_l;
}


pointNode* getRandomStart(GLfloat xMin, GLfloat xMax, GLfloat yMin, GLfloat yMax)
{
	struct pointNode* retVal;
	GLfloat xLen = xMax - xMin;
	GLfloat yLen = yMax - yMin;

	GLfloat startX = ((GLfloat)rand() / (GLfloat)RAND_MAX) * xLen + xMin;
	GLfloat startY = ((GLfloat)rand() / (GLfloat)RAND_MAX) * xLen + xMin;

	retVal = (pointNode*)malloc(sizeof(pointNode));
	retVal->x = startX;
	retVal->y = startY;
	retVal->next = NULL;

	return retVal;

}

pointNode* AddNode(struct pointNode* node, GLfloat x, GLfloat y)
{
	struct pointNode* tmp = NULL;
	while (node->next != NULL)
	{
		node = node->next;
	}

	tmp = (pointNode *)malloc(sizeof(pointNode));
	tmp->x = x;
	tmp->y = y;

	tmp->next = NULL;
	node->next = tmp;

	return tmp;
}

//this function will determine the length of the displacement vectors
//it will be 1/50 the shortest side.
GLfloat calcDisplacement(GLfloat xMin, GLfloat xMax, GLfloat yMin, GLfloat yMax)
{
	GLfloat lenX = xMax - xMin;
	GLfloat lenY = yMax - yMin;

	if (lenX < lenY)
	{
		return lenX / 50.0;
	}
	else
	{
		return lenY / 50.0;
	}
}
//this function will check to see if two segments collide with each other. The second segment is parallel with X axis or Y axis.
bool Collide(struct pointNode * pt1, struct pointNode * pt2, struct pointNode * pt3, struct pointNode * pt4)
{
	GLfloat slope;
	GLfloat b; // y -intercept of  the line y = slope*x + b
	slope = (pt2->y - pt1->y) / (pt2->x - pt1->x);
	b = pt1->y - slope * pt1->x;
	GLfloat xExit, yExit;
	if (pt3->x == pt4->x)
	{
		GLfloat yRangeMin, yRangeMax;
		if (pt3->y > pt4->y)
		{
			yRangeMax = pt3->y;
			yRangeMin = pt4->y;
		}
		else
		{
			yRangeMin = pt3->y;
			yRangeMax = pt4->y;
		}
		if (pt1->x > pt3->x && pt2->x > pt3->x) return false;
		if (pt1->x < pt3->x && pt2->x < pt3->x) return false;
		yExit = slope * pt3->x + b;
		if (yExit > yRangeMin && yExit < yRangeMax)
		{
			xLast = pt3->x;
			yLast = yExit;
			return true;
		}		
	}
	if (pt3->y == pt4->y)
	{
		GLfloat xRangeMin, xRangeMax;
		if (pt3->x > pt4->x)
		{
			xRangeMin = pt4->x;
			xRangeMax = pt3->x;
		}
		else
		{
			xRangeMax = pt4->x;
			xRangeMin = pt3->x;
		}
		if (pt1->y > pt3->y && pt2->y > pt3->y) return false;
		if (pt1->y < pt3->y && pt2->y < pt3->y) return false;
		xExit = (pt3->y - b) / slope;
		if (xExit > xRangeMin && xExit < xRangeMax)
		{
			xLast = xExit;
			yLast = pt3->y;
			return true;
		}
	}
	return false;
}
//this function will check to see if the node is withing the box
bool checkNode(struct pointNode * prev, struct pointNode * curr, GLfloat xMin, GLfloat xMax, GLfloat yMin, GLfloat yMax)
{	
	int i;
	struct pointNode pt1, pt2;
	pt1.x = xMin, pt1.y = yMin, pt2.x = xMax, pt2.y  = yMin;
	if (Collide(prev, curr, &pt1, &pt2)) return false;

	pt1.x = xMax, pt1.y = yMin, pt2.x = xMax, pt2.y = yMax;
	if (Collide(prev, curr, &pt1, &pt2)) return false;

	pt1.x = xMax, pt1.y = yMax, pt2.x = xMin, pt2.y = yMax;
	if (Collide(prev, curr, &pt1, &pt2)) return false;

	pt1.x = xMin, pt1.y = yMax, pt2.x = xMin, pt2.y = yMin;
	if (Collide(prev, curr, &pt1, &pt2)) return false;

	for (i = 0; i < 7; i++)
	{
		pt1.x = points1[i][0], pt1.y = points1[i][1];
		pt2.x = points1[i+1][0], pt2.y = points1[i+1][1];
		if (Collide(prev, curr, &pt1, &pt2)) return false;
	}
	return true;
}
//Get minimum distance of starting point from the walls.
GLfloat
GetMinDistance()
{
	GLfloat d = width;
	GLfloat x, y;
	struct pointNode pt1, pt2;
	x = head->x, y = head->y;
	int i;
	GLfloat distance;
	for (i = 0; i < 7; i++)
	{
		distance = d;
		pt1.x = points1[i][0], pt1.y = points1[i][1];
		pt2.x = points1[i + 1][0], pt2.y = points1[i + 1][1];
		if (pt1.x == pt2.x)	distance = fabs(pt1.x - x);		
		if (pt1.y == pt2.y) distance = fabs(pt1.y - y);
		if (distance < d) d = distance;
	}
	return d;
}
// OpenGL initialization
void
init()
{
	//Start the Brownian motion
	head = getRandomStart(minX, maxX, minY, maxY);
	curr = head;

    // Create and initialize a buffer object
    
    glGenBuffers( 3, &buffers[0] );
	
    glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points0),  points0, GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, buffers[1] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points1),  points1, GL_STATIC_DRAW );

   // Load shaders and use the resulting shader program
	GLuint program = InitShader( "vshader00_v110.glsl", "fshader00_v110.glsl" );    
    glUseProgram( program );

    
    vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
	
	projmat_loc = glGetUniformLocation(program, "projmat");
	color_loc = glGetUniformLocation(program, "color");
	model_view_loc = glGetUniformLocation(program, "modelview");

    //glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); 

	width = maxX - minX;
	height = maxY - minY;	
	//displacement = GetMinDistance() / 5.0; //This is very slow.
	displacement = width / 50.0;
}

//----------------------------------------------------------------------------


void
display1( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	projmat = Angel::mat4(1.0);
	glUniformMatrix4fv(projmat_loc, 1, GL_TRUE, projmat);

	glUniform4fv(color_loc, 1, color4(1.0, 1.0, 0.0, 1.0));

	//Draw labyrinth
	ctmat = Angel::mat4(1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, ctmat);
	glBindBuffer( GL_ARRAY_BUFFER, buffers[1] );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	glDrawArrays( GL_LINE_STRIP, 0, 8);
	
	glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	glDrawArrays(GL_LINES, 0, 2 );

	//Mark initial location
	
	glUniform4fv( color_loc, 1, green_start_marker );
	glRectf(head->x - width / 50.0, head->y - height / 50.0f, head->x + width / 50.0f, head->y + height / 50.0f);


	//Draw trajectory
	modelview = Angel::mat4(1.0);
	glUniformMatrix4fv(modelview_loc, 1, GL_TRUE, modelview);
	//copy the trajectory into a buffer 
	GLfloat * trajectoryBuffer = copyToArray(head);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * pointCount(head), trajectoryBuffer, GL_STREAM_DRAW);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glUniform4fv(color_loc, 1, blue_trajectory);
	glDrawArrays(GL_LINE_STRIP, 0, pointCount(head));

	glutSwapBuffers();

	delete[] trajectoryBuffer;

	if (bComplete)
	{
		GLfloat linewidth = width / 25.0f;
		glUniform4fv(color_loc, 1, red_exit_marker);
		glBegin(GL_LINES);
		glVertex3f(xLast - linewidth / 2, yLast - linewidth / 2, 0.0);
		glVertex3f(xLast + linewidth / 2, yLast + linewidth / 2, 0.0);		
		glVertex3f(xLast - linewidth / 2, yLast + linewidth / 2, 0.0);
		glVertex3f(xLast + linewidth / 2, yLast - linewidth / 2, 0.0);
		glEnd();		
	}
	
	glFlush();
}


//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 'p':
	{
		bPaused = !bPaused;
		if (!bPaused)
			animate(0);
	}
		break;
	case 033:  // Escape key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    }
}

GLfloat* copyToArray(struct pointNode * head)
{
	GLfloat * retVal;
	pointNode * tmp;
	int count_l;
	count_l = pointCount(head);
	int i = 0;

	count_l *= 2;

	tmp = head;
	if (count_l > 0)
	{

		retVal = new GLfloat[count_l];
	}
	else
	{
		return NULL;
	}

	while (i < count_l)
	{
		retVal[i] = tmp->x;
		retVal[i + 1] = tmp->y;
		tmp = tmp->next;
		i += 2;
	}
	return retVal;
}

void animate(int i)
{
	GLfloat x, y;
	pointNode * last;
	if (!bPaused && !bComplete)
	{
		last = curr;
		randomDisplacement(displacement, x, y);
		curr = AddNode(curr, curr->x + x, curr->y + y);
		int count_l;
		count_l = pointCount(head);

		//We only want to keep going if 
		if (checkNode(last, curr, minX, maxX, minY, maxY))
		{
			//keep a roughly constat fps
			glutTimerFunc(17, animate, 0);
		}
		else
		{			
			bComplete = true;
		}
		glutPostRedisplay();
	}
}
//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
	struct pointNode* tmp = NULL;

	srand(time(NULL));
	rand();

	height = maxY - minY;
	width = maxX - minX;

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowSize( 512, 512 );

    int noscaled = glutCreateWindow( "Silly Labyrinth" );
	glutInitWindowPosition(256, 256);
	m_glewInitAndVersion();
	init();
	glutDisplayFunc( display1 );
    glutKeyboardFunc( keyboard );
	glutTimerFunc(1000, animate, 0);

    glutMainLoop();
    return 0;
}