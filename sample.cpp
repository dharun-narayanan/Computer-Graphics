#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

#define XSIDE	80.0f		// length of the x side of the grid
#define X0      (-XSIDE/2.)		// where one side starts
#define NX	1000			// how many points in x
#define DX	( XSIDE/(float)NX )	// change in x between the points

#define YGRID	-2.6f

#define ZSIDE	80.0f				// length of the z side of the grid
#define Z0      (-ZSIDE/2.)		// where one side starts
#define NZ	1000			// how many points in z
#define DZ	( ZSIDE/(float)NZ )	// change in z between the points


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//


// title of these windows:

const char* WINDOWTITLE = "OpenGL / GLUT Sample -- Dharun Graphics";
const char* GLUITITLE = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 800;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT = 4;
const int MIDDLE = 2;
const int RIGHT = 1;
bool Frozen;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char* ColorNames[] =
{
	(char*)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE = GL_LINEAR;
const GLfloat FOGDENSITY = 0.30f;
const GLfloat FOGSTART = 1.5f;
const GLfloat FOGEND = 4.f;

// for lighting:

const float	WHITE[] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 10000;		// 10000 milliseconds = 10 seconds


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
GLuint	GridDL;				// object display list
GLuint	GridDL1;
GLuint	Spaceship;			// object display list
GLuint	Sphere;
GLuint	Sphere1;
GLuint	Sphere2;
GLuint	Torus;
GLuint	LightSource;
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		NowColor;				// index into Colors[ ]
int		NowProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
GLfloat lightType = GL_LIGHT0; // Initialize as point light
GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // White color
GLfloat lightAngle = 0.0f; // Angle for circular motion
bool lookstate = 1;
bool lightstate = 1;
int shadetype = 0;



// function prototypes:

void	Animate();
void	Display();
void	DoAxesMenu(int);
void	DoColorMenu(int);
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoRasterString(float, float, float, char*);
void	DoStrokeString(float, float, float, float, char*);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);
void	DoViewMenu(int);

void			Axes(float);
void			HsvRgb(float[3], float[3]);
void			Cross(float[3], float[3], float[3]);
float			Dot(float[3], float[3]);
float			Unit(float[3], float[3]);
float			Unit(float[3]);

// utility to create an array from 3 separate values:

float*
Array3(float a, float b, float c)
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float*
MulArray3(float factor, float array0[])
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float*
MulArray3(float factor, float a, float b, float c)
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}

// these are here for when you need them -- just uncomment the ones you need:

#include "setmaterial.cpp"
#include "setlight.cpp"
#include "osusphere.cpp"
//#include "osucone.cpp"
#include "osutorus.cpp"
//#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
//#include "glslprogram.cpp"

#define MSEC 20000
Keytimes Xpos, Ypos, Zpos;
Keytimes ThetaX, ThetaY, ThetaZ;
Keytimes SphereX, SphereZ;
Keytimes TorusX, TorusY, TorusZ;
Keytimes Kred, Kgreen, Kblue;
Keytimes lookX, lookY, lookZ;
Keytimes eyeX, eyeY, eyeZ;
Keytimes lsourceX, lsourceY, lsourceZ;
Keytimes lydir;
Keytimes ScaleX, ScaleY, ScaleZ;
float loopDuration = 20.0;
float totalDuration = 2 * loopDuration;

// main program:

int
main(int argc, char* argv[])
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit(&argc, argv);

	// setup all the graphics stuff:

	InitGraphics();

	// create the display lists that **will not change**:

	InitLists();

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset();

	// setup all the user interface stuff:

	InitMenus();

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow(MainWindow);
	glutMainLoop();

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate()
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.






	glutSetWindow(MainWindow);
	glutPostRedisplay();
}




// draw the complete scene:

void
Display()
{
	int msec = glutGet(GLUT_ELAPSED_TIME) % MSEC;
	float nowSecs = (float)msec / 1000.f;

	

	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow(MainWindow);

	// erase the background:
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
#ifdef DEMO_DEPTH_BUFFER
	if (DepthBufferOn == 0)
		glDisable(GL_DEPTH_TEST);
#endif


	// specify shading to be flat:

	if (shadetype == 0)
		glShadeModel(GL_FLAT);
	else
		glShadeModel(GL_SMOOTH);



	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (NowProjection == ORTHO)
		glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
	else
		gluPerspective(70.f, 1.f, 0.1f, 1000.f);

	// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set the eye position, look-at position, and up-vector:

	//gluLookAt(6, 20, 30, 0, 0, 0, 0, 1, 0);
	if (lookstate != 0)
		gluLookAt(eyeX.GetValue(nowSecs), eyeY.GetValue(nowSecs), eyeZ.GetValue(nowSecs), lookX.GetValue(nowSecs), lookY.GetValue(nowSecs), lookZ.GetValue(nowSecs), 0, 1, 0);
	else
		gluLookAt(6, 20, 30, 0, 0, 0, 0, 1, 0);
	

	// rotate the scene:

	glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
	glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);

	// uniformly scale the scene:

	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

	// set the fog parameters:

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}

	// possibly draw the axes:

	if (AxesOn != 0)
	{
		glColor3fv(&Colors[NowColor][0]);
		glCallList(AxesList);
	}


	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if (lightstate != 0)
		SetPointLight(GL_LIGHT0, lsourceX.GetValue(nowSecs), lsourceY.GetValue(nowSecs), lsourceZ.GetValue(nowSecs), lightColor[0], lightColor[1], lightColor[2]);
	else
		SetSpotLight(GL_LIGHT0, lsourceX.GetValue(nowSecs), lsourceY.GetValue(nowSecs), lsourceZ.GetValue(nowSecs), 0, lydir.GetValue(nowSecs), 0, lightColor[0], lightColor[1], lightColor[2]);



	glCallList(GridDL);
	glCallList(GridDL1);

	glPushMatrix(); 
		SetMaterial(Kred.GetValue(nowSecs), 1, Kgreen.GetValue(nowSecs), 1.f);
		glTranslatef(Xpos.GetValue(nowSecs), Ypos.GetValue(nowSecs), Zpos.GetValue(nowSecs));
		glRotatef(ThetaZ.GetValue(nowSecs), 0., 0., 1.);
		glRotatef(ThetaY.GetValue(nowSecs), 0., 1., 0.);
		glScalef(ScaleX.GetValue(nowSecs), ScaleY.GetValue(nowSecs), ScaleZ.GetValue(nowSecs));
		glCallList(Spaceship);
	glPopMatrix();

	glPushMatrix();
		SetMaterial(Kred.GetValue(nowSecs), Kgreen.GetValue(nowSecs), Kblue.GetValue(nowSecs), 10.f);
		glTranslatef(SphereX.GetValue(nowSecs), 0, SphereZ.GetValue(nowSecs));
		glTranslatef(Xpos.GetValue(nowSecs), Ypos.GetValue(nowSecs), Zpos.GetValue(nowSecs));
		glCallList(Sphere);
	glPopMatrix();

	glPushMatrix();
		SetMaterial(Kred.GetValue(nowSecs), Kgreen.GetValue(nowSecs), 1, 10.f);
		glTranslatef(Xpos.GetValue(nowSecs), -Ypos.GetValue(nowSecs), Zpos.GetValue(nowSecs));
		glRotatef(TorusZ.GetValue(nowSecs), 0., 0., 1.);
		glRotatef(TorusY.GetValue(nowSecs), 0., 1., 0.);
		glCallList(Torus);
	glPopMatrix();

	
	glPushMatrix();
	SetMaterial(1, Kgreen.GetValue(nowSecs), Kred.GetValue(nowSecs), 5.f);
	glCallList(Sphere1);
	glPopMatrix();

	glPushMatrix();
	SetMaterial(1, Kgreen.GetValue(nowSecs), Kred.GetValue(nowSecs), 5.f);
	glCallList(Sphere2);
	glPopMatrix();

	glPushMatrix();
	glShadeModel(GL_SMOOTH);
	glColor3fv(lightColor);
	glTranslatef(lsourceX.GetValue(nowSecs), lsourceY.GetValue(nowSecs), lsourceZ.GetValue(nowSecs));
	glCallList(LightSource);
	glPopMatrix();


#ifdef DEMO_Z_FIGHTING
	if (DepthFightingOn != 0)
	{
		glPushMatrix();
		glRotatef(90.f, 0.f, 1.f, 0.f);
		glCallList(GridDL);
		glPopMatrix();
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	glDisable(GL_DEPTH_TEST);
	glColor3f(0.f, 1.f, 1.f);
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.f, 100.f, 0.f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.f, 1.f, 1.f);
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers();

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush();
}


void
DoAxesMenu(int id)
{
	AxesOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoColorMenu(int id)
{
	NowColor = id - RED;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDebugMenu(int id)
{
	DebugOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthBufferMenu(int id)
{
	DepthBufferOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthFightingMenu(int id)
{
	DepthFightingOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoShadeMenu(int id)
{
	shadetype = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthMenu(int id)
{
	DepthCueOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// main menu callback:

void
DoMainMenu(int id)
{
	switch (id)
	{
	case RESET:
		Reset();
		break;

	case QUIT:
		// gracefully close out the graphics:
		// gracefully close the graphics window:
		// gracefully exit the program:
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoProjectMenu(int id)
{
	NowProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// use glut to display a string of characters using a raster font:

void
DoRasterString(float x, float y, float z, char* s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString(float x, float y, float z, float ht, char* s)
{
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds()
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow(MainWindow);

	int numColors = sizeof(Colors) / (3 * sizeof(float));
	int colormenu = glutCreateMenu(DoColorMenu);
	for (int i = 0; i < numColors; i++)
	{
		glutAddMenuEntry(ColorNames[i], i);
	}

	int axesmenu = glutCreateMenu(DoAxesMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthcuemenu = glutCreateMenu(DoDepthMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int debugmenu = glutCreateMenu(DoDebugMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int shademenu = glutCreateMenu(DoShadeMenu);
	glutAddMenuEntry("Smooth", 1);
	glutAddMenuEntry("Flat", 0);

	int mainmenu = glutCreateMenu(DoMainMenu);
	glutAddSubMenu("Shading", shademenu);
	glutAddSubMenu("Axes", axesmenu);
	glutAddSubMenu("Axis Colors", colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu("Depth Buffer", depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu("Depth Fighting", depthfightingmenu);
#endif

	glutAddSubMenu("Depth Cue", depthcuemenu);
	glutAddMenuEntry("Reset", RESET);
	glutAddSubMenu("Debug", debugmenu);
	glutAddMenuEntry("Quit", QUIT);

	// attach the pop-up menu to the right mouse button:

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc(Animate);


	Xpos.Init();
	Ypos.Init();
	Zpos.Init();
	ThetaX.Init();
	ThetaY.Init();
	ThetaZ.Init();
	SphereX.Init();
	SphereZ.Init();
	lookX.Init();
	lookY.Init();
	lookZ.Init();
	eyeX.Init();
	eyeY.Init();
	eyeZ.Init();
	Kred.Init();
	Kgreen.Init();
	Kblue.Init();
	lsourceX.Init();
	lsourceY.Init();
	lsourceZ.Init();
	lydir.Init();
	ScaleX.Init();
	ScaleY.Init();
	ScaleZ.Init();


	float zrotate = 0.f;
	float torusZ = 0.f;
	float sphereY = 0.f;

	for (float t = 0.0; t <= loopDuration; t += 1.0) 
	{
		float xpos = 20.0 * cos(t / loopDuration * 2 * M_PI);
		float ypos = 20.0 * sin(2 * t / loopDuration * 2 * M_PI);
		float zpos = 0.0 * sin(t / loopDuration * 2 * M_PI);

		float xlook = xpos;
		float ylook = ypos;
		float zlook = zpos;

		float xeye = xpos;
		float yeye = ypos;
		float zeye = zpos + 40;


		if (t>=1.f && t<=2.5f)
		{
			xeye = xpos - 8;
			yeye = ypos - 8;
		}
		else if (t > 2.5f && t < 7.5f)
		{
			xeye = xpos + 8;
			yeye = ypos + 8;
			zeye = zpos;
		}
		else if(t >= 7.5f && t < 10.f)
		{
			xeye = xpos - 8;
			yeye = ypos - 8;
			zeye = zpos + 40;
			
		}
		else if (t >= 10.f && t < 17.5f)
		{
			xeye = xpos + 8;
			yeye = ypos + 8;
			zeye = zpos;
		}
		else if (t >= 17.5f && t <= 19.f)
		{
			xeye = xpos - 8;
			yeye = ypos - 8;
			zeye = zpos + 20;
		}
		else
		{
			xeye = xpos + 8;
			yeye = ypos + 8;
			zeye = zpos + 20;
		}

		Xpos.AddTimeValue(t, xpos);
		Ypos.AddTimeValue(t, ypos);
		Zpos.AddTimeValue(t, zpos);

		lookX.AddTimeValue(t, xlook);
		lookY.AddTimeValue(t, ylook);
		lookZ.AddTimeValue(t, zlook);

		eyeX.AddTimeValue(t, xeye);
		eyeY.AddTimeValue(t, yeye);
		eyeZ.AddTimeValue(t, zeye);

	}
	
	for (float t = 0.0; t <= loopDuration; t += 1.0)
	{
		// Calculate desired rotations (e.g., for orientation)
		float xrotate = t * 36;

		float torusY = t * 36;

		float sphereX = 5.0 * cos(2.0f * M_PI * t / 5.f);
		float sphereZ = -5.0 * sin(2.0f * M_PI * t / 5.f);

		

		if (t<=5)
		{			
			zrotate = t * 36;
			torusZ = -t * 36;
			sphereY = zrotate;
		}
		else if (t > 5 && t <= 15)
		{
			zrotate = zrotate - 36;
			torusZ = torusZ + 36;
			sphereY = zrotate;
		}
		else
		{
			zrotate = zrotate + 36;
			torusZ = torusZ - 36;
			sphereY = zrotate;
		}

		ThetaX.AddTimeValue(t, xrotate);
		ThetaZ.AddTimeValue(t, zrotate);

		TorusY.AddTimeValue(t, torusY);
		TorusZ.AddTimeValue(t, torusZ);

		SphereX.AddTimeValue(t, sphereX);
		SphereZ.AddTimeValue(t, sphereZ);

		
	}	 
	
	for (float t = 0; t <= loopDuration; t += 1)
	{
		float red = 0.5 * t;
		float green = 0.5 * (20-t);
		float blue = 0.5 * t;

		Kred.AddTimeValue(t, red);
		Kgreen.AddTimeValue(t, green);
		Kblue.AddTimeValue(t, blue);

	}

	lsourceX.AddTimeValue(0, 20);
	lsourceY.AddTimeValue(0, 20);
	lsourceZ.AddTimeValue(0, 5);
	lydir.AddTimeValue(0, -2.2f);

	lsourceX.AddTimeValue(5, 0);
	lsourceY.AddTimeValue(5, 20);
	lsourceZ.AddTimeValue(5, -5);
	lydir.AddTimeValue(5, -2.2f);


	lsourceX.AddTimeValue(7.5, -10);
	lsourceY.AddTimeValue(7.5, -20);
	lsourceZ.AddTimeValue(7.5, 5);
	lydir.AddTimeValue(7.5, 1.f);

	lsourceX.AddTimeValue(10, -20);
	lsourceY.AddTimeValue(10, 20);
	lsourceZ.AddTimeValue(10, -5);
	lydir.AddTimeValue(10, -2.2f);

	lsourceX.AddTimeValue(12.5, -10);
	lsourceY.AddTimeValue(12.5, 20);
	lsourceZ.AddTimeValue(12.5, 5);
	lydir.AddTimeValue(12.5, -2.2f);

	lsourceX.AddTimeValue(15, 0);
	lsourceY.AddTimeValue(15, 20);
	lsourceZ.AddTimeValue(15, -5);
	lydir.AddTimeValue(15, -2.2f);

	lsourceX.AddTimeValue(17.5, 10);
	lsourceY.AddTimeValue(17.5, -20);
	lsourceZ.AddTimeValue(17.5, 5);
	lydir.AddTimeValue(17.5, 1.f);

	lsourceX.AddTimeValue(20, 20);
	lsourceY.AddTimeValue(20, 20);
	lsourceZ.AddTimeValue(20, -5);
	lydir.AddTimeValue(20, -2.2f);

	ScaleX.AddTimeValue(0, 1);
	ScaleY.AddTimeValue(0, 1);
	ScaleZ.AddTimeValue(0, 1);

	ScaleX.AddTimeValue(5, 0.5);
	ScaleY.AddTimeValue(5, 0.5);
	ScaleZ.AddTimeValue(5, 0.5);

	ScaleX.AddTimeValue(7.5, 1.5);
	ScaleY.AddTimeValue(7.5, 1.5);
	ScaleZ.AddTimeValue(7.5, 1.5);

	ScaleX.AddTimeValue(10, 0.5);
	ScaleY.AddTimeValue(10, 0.5);
	ScaleZ.AddTimeValue(10, 0.5);

	ScaleX.AddTimeValue(12.5, 1.5);
	ScaleY.AddTimeValue(12.5, 1.5);
	ScaleZ.AddTimeValue(12.5, 1.5);

	ScaleX.AddTimeValue(15, 0.5);
	ScaleY.AddTimeValue(15, 0.5);
	ScaleZ.AddTimeValue(15, 0.5);

	ScaleX.AddTimeValue(20, 1);
	ScaleY.AddTimeValue(20, 1);
	ScaleZ.AddTimeValue(20, 1);


	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");


	glutSetWindow(MainWindow);

	GridDL = glGenLists(1);
	glNewList(GridDL, GL_COMPILE);
	glPushMatrix();
	SetMaterial(0.6f, 0.6f, 0.6f, 30.f);
	glNormal3f(0., 1., 0.);
	for (int i = 0; i < NZ; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < NX; j++)
		{
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 0));
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 1));
		}
		glEnd();
	}
	glPopMatrix();
	glEndList();

	GridDL1 = glGenLists(1);
	glNewList(GridDL1, GL_COMPILE);
	glPushMatrix();
	SetMaterial(0.6f, 0.6f, 0.6f, 30.f);
	glNormal3f(0., -1., 0.);
	for (int i = 0; i < NZ; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < NX; j++)
		{
			glVertex3f(X0 + DX * (float)j, YGRID - 1, Z0 + DZ * (float)(i + 0));
			glVertex3f(X0 + DX * (float)j, YGRID - 1, Z0 + DZ * (float)(i + 1));
		}
		glEnd();
	}
	glPopMatrix();
	glEndList();


	Spaceship = glGenLists(1);
	glNewList(Spaceship, GL_COMPILE);
	glPushMatrix();
	glTranslatef(0.f, 2.2f, 0.f);
	glScalef(0.8, 0.8, 0.8);
	glRotatef(90.f, 1.f, 0.f, 0.f);
	LoadObjFile((char*)"prometheus.obj");
	glPopMatrix();
	glEndList();

	Sphere = glGenLists(1);
	glNewList(Sphere, GL_COMPILE);
	glPushMatrix();
	OsuSphere(0.5,20,20);
	glPopMatrix();
	glEndList();

	Sphere1 = glGenLists(1);
	glNewList(Sphere1, GL_COMPILE);
	glPushMatrix();
	glTranslatef(-10.f, -3.f, 0.f);
	OsuSphere(5, 20, 20);
	glPopMatrix();
	glEndList();

	Sphere2 = glGenLists(1);
	glNewList(Sphere2, GL_COMPILE);
	glPushMatrix();
	glTranslatef(10.f, -3.f, 0.f);
	OsuSphere(5, 20, 20);
	glPopMatrix();
	glEndList();

	Torus = glGenLists(1);
	glNewList(Torus, GL_COMPILE);
	glPushMatrix();
	OsuTorus(1.0, 3.0, 20, 20);
	glPopMatrix();
	glEndList();

	LightSource = glGenLists(1);
	glNewList(LightSource, GL_COMPILE);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	OsuSphere(0.2, 10, 10); // Draw a small sphere
	glPopMatrix();
	glEndList();

	// create the axes:
	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glLineWidth(AXES_WIDTH);
	Axes(1.5);
	glLineWidth(1.);
	glEndList();
}


// the keyboard callback:



void
Keyboard(unsigned char c, int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case 'P':
	case 'p':
		lightstate = 1;
		break;
	case 'S':
	case 's':
		lightstate = 0;
		break;
	case 'Z':
	case 'z':
		lookstate = 1;
		break;
	case 'X':
	case 'x':
		lookstate = 0;
		break;
	case 'W':
	case 'w':
		lightColor[0] = 1.0f; // Red
		lightColor[1] = 1.0f; // Green
		lightColor[2] = 1.0f; // Blue
		break;
	case 'R':
	case 'r':
		lightColor[0] = 1.0f; // Red
		lightColor[1] = 0.0f; // Green
		lightColor[2] = 0.0f; // Blue
		break;
	case 'G':
	case 'g':
		lightColor[0] = 0.0f; // Red
		lightColor[1] = 1.0f; // Green
		lightColor[2] = 0.0f; // Blue
		break;
	case 'B':
	case 'b':
		lightColor[0] = 0.0f; // Red
		lightColor[1] = 0.0f; // Green
		lightColor[2] = 1.0f; // Blue
		break;
	case 'Y':
	case 'y':
		lightColor[0] = 1.0f; // Red
		lightColor[1] = 1.0f; // Green
		lightColor[2] = 0.0f; // Blue
		break;
	case 'f':
	case 'F':
		Frozen = !Frozen;
		if (Frozen)
			glutIdleFunc(NULL);
		else
			glutIdleFunc(Animate);
		break;

	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler

	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);


	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	case SCROLL_WHEEL_UP:
		Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	case SCROLL_WHEEL_DOWN:
		Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}

	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT * dy);
		Yrot += (ANGFACT * dx);
	}

	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset()
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
	Frozen = false;
}


// called when user resizes the window:

void
Resize(int width, int height)
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// handle a change to the window's visibility:

void
Visibility(int state)
{
	if (DebugOn != 0)
		fprintf(stderr, "Visibility: %d\n", state);

	if (state == GLUT_VISIBLE)
	{
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = { 0.f, 1.f, 0.f, 1.f };

static float xy[] = { -.5f, .5f, .5f, -.5f };

static int xorder[] = { 1, 2, -3, 4 };

static float yx[] = { 0.f, 0.f, -.5f, .5f };

static float yy[] = { 0.f, .6f, 1.f, 1.f };

static int yorder[] = { 1, 2, 3, -2, 4 };

static float zx[] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes(float length)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++)
	{
		int j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++)
	{
		int j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++)
	{
		int j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
	}
	glEnd();

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb(float hsv[3], float rgb[3])
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while (h >= 6.)	h -= 6.;
	while (h < 0.) 	h += 6.;

	float s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	float v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;

	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = (float)floor(h);
	float f = h - i;
	float p = v * (1.f - s);
	float q = v * (1.f - s * f);
	float t = v * (1.f - (s * (1.f - f)));

	float r = 0., g = 0., b = 0.;			// red, green, blue
	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit(float v[3])
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}
