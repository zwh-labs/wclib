#define SDL_MAIN_HANDLED

#include <wc/Configuration.h>
#include <wc/Configuration_stdio.h>
#include <wc/Configuration_ArgumentParser.h>
#include <wc/Connection.h>
#include <wc/Connection_Configuration.h>
#include <wc/Thread.h>
#include <wc/WheelMovement.h>
#include <wc/WheelMovement_Configuration.h>
#include <wc/WheelPIDController.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <GL/glu.h>

#include <stdio.h>
#include <stdbool.h>


static SDL_Window * window = NULL;
static SDL_GLContext glContext = 0;

static bool init_sdl( int argc, const char ** argv )
{
	(void)argc;
	(void)argv;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		fprintf( stderr, "Failed to initialize SDL: %s\n", SDL_GetError() );
		return false;
	}

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

	window = SDL_CreateWindow( "WheelChar SDL2/OpenGL Example PID Control", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	if( !window )
	{
		fprintf( stderr, "Failed to create Window: %s\n", SDL_GetError() );
		return false;
	}

	glContext = SDL_GL_CreateContext( window );
	if( !glContext )
	{
		fprintf( stderr, "Failed to create OpenGL context: %s\n", SDL_GetError() );
		return false;
	}

	if( SDL_GL_SetSwapInterval( 1 ) < 0 )
	{
		fprintf( stderr, "Unable to set swap interval: %s\n", SDL_GetError() );
	}

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable(GL_DEPTH_TEST);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	return true;
}


static wcConfiguration * configuration = NULL;
static wcConnection * connection = NULL;
static wcThread * thread = NULL;

static bool init_wc( int argc, const char ** argv )
{
	configuration = wcConfiguration_newFromArguments( argc, argv );
	if( !configuration )
	{
		fprintf( stderr, "Failed to read configuration\n" );
		return false;
	}
	wcConfiguration_fprint( stderr, configuration );

	connection = wcConnection_openFromConfiguration( configuration );
	if( !connection )
	{
		fprintf( stderr, "Failed to open connection\n" );
		return false;
	}

	thread = wcThread_start( connection );
	if( !thread )
	{
		fprintf( stderr, "Failed to start thread\n" );
		return false;
	}

	return true;
}


typedef struct
{
	wcWheelPIDController * controller;
	int targetAngleIncrements;
	double rotationAngle;
	double velocity;
} Wheel;
static unsigned int wheelCount = 0;
static Wheel * wheels = NULL;

static bool init( int argc, const char ** argv )
{
	if( !init_wc( argc, argv ) )
		return false;

	if( !init_sdl( argc, argv ) )
		return false;

	wheelCount = wcConfiguration_getWheelCount( configuration );
	wheels = malloc( sizeof(Wheel) * wheelCount );
	for( unsigned int i=0; i<wheelCount; i++ )
	{
		wheels[i].controller = wcWheelPIDController_new();
		if( !wheels[i].controller )
		{
			fprintf( stderr, "Failed to instantiate PID controller\n" );
			return false;
		}
		wheels[i].rotationAngle = 0.0;
		wheels[i].targetAngleIncrements = 0;
		wheels[i].velocity = 0.0;
	}

	return true;
}


// http://stackoverflow.com/questions/12089514/real-modulo-operator-in-c-c
static inline int modulo( int a, int b )
{
	const int result = a % b;
	return result >= 0 ? result : result + b;
}


static inline double fraction( double d )
{
	return d - floor(d);
}


static void update()
{
	static unsigned int lastTime = 0;
	static unsigned int currentTime = 0;

	currentTime = SDL_GetTicks();
	double delta = (double)(currentTime - lastTime) / 1000.0;
	lastTime = currentTime;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	for( unsigned int i=0; i<wheelCount; i++ )
	{
		int incrementsPerTurn = wcConfiguration_getWheelIncrementsPerTurn( configuration, i );

		wcWheelMovement wm = wcThread_retrieveWheelMovement( thread, i );
		wheels[i].targetAngleIncrements += wcWheelMovement_getIncrements( &wm );
		wheels[i].targetAngleIncrements = modulo( wheels[i].targetAngleIncrements, incrementsPerTurn );

		int actualAngleIncrements = wheels[i].rotationAngle * (double)incrementsPerTurn;
		double controlValue = wcWheelPIDController_updateAngular( wheels[i].controller, wheels[i].targetAngleIncrements, actualAngleIncrements, incrementsPerTurn, delta );
		wheels[i].velocity += controlValue;
		wheels[i].rotationAngle += wheels[i].velocity;
		wheels[i].rotationAngle = fraction(wheels[i].rotationAngle);
		glPushMatrix();
			unsigned char color = i+1;
			glColor3f( color&2, color&1, color&4 );
			glRotatef( wheels[i].rotationAngle * 360.0f, 0,0,1 );
			glBegin( GL_QUADS );
				glVertex3f( -0.5f, -0.5f, i*0.5f );
				glVertex3f( 0.5f, -0.5f, i*0.5f );
				glVertex3f( 0.5f, 0.5f, i*0.5f );
				glVertex3f( -0.5f, 0.5f, i*0.5f );
			glEnd();
		glPopMatrix();
	}
}


static void quit_sdl()
{
	SDL_GL_DeleteContext( glContext );
	glContext = 0;
	SDL_DestroyWindow( window );
	window = NULL;
	SDL_Quit();
}


static void quit_wc()
{
	wcThread_stop( thread );
	thread = NULL;
	wcConnection_close( connection );
	connection = NULL;
	wcConfiguration_delete( configuration );
	configuration = NULL;
	for( unsigned int i=0; i<wheelCount; i++ )
		wcWheelPIDController_delete( wheels[i].controller );
}


static void quit()
{
	quit_sdl();
	quit_wc();
	free( wheels );
}


int main( int argc, const char ** argv )
{
	if( !init( argc, argv ) )
		return 1;

	bool run = true;
	while( run )
	{
		SDL_Event e;
		while( SDL_PollEvent( &e ) )
		{
			switch( e.type )
			{
			case SDL_QUIT:
				run = false;
				break;
			case SDL_WINDOWEVENT:
				switch( e.window.event )
				{
				case SDL_WINDOWEVENT_RESIZED:
					glViewport( 0, 0, e.window.data1, e.window.data2 );
					break;
				}
				break;
			}
		}
		update();
		SDL_GL_SwapWindow( window );
	}

	quit();
	return 0;
}
