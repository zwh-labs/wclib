/* This example emulates wheels and sends the packets to a serial console.
 * Example usage:
 * - Create a pair of pseudo terminals using
 *     # socat -d -d pty,raw,echo=0 pty,raw,echo=0
 *   this will create a for example /dev/pts/4 and /dev/pts/5
 * - Run the emulator on one of the pseudo terminals
 *     # ./wcExampleSDL2OpenGLWCEmulator --devicePath /dev/pts/5
 * - Now connect another example to the other side
 *     # ./wcExampleSDL2OpenGLPID --devicePath /dev/pts/4
 */

#define SDL_MAIN_HANDLED

#include <wc/Configuration.h>
#include <wc/Configuration_stdio.h>
#include <wc/Configuration_ArgumentParser.h>
#include <wc/Connection.h>
#include <wc/Connection_Configuration.h>
#include <wc/WheelMovement.h>
#include <wc/WheelMovement_Configuration.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <GL/glu.h>

#include <stdio.h>
#include <stdbool.h>
#include <math.h>


typedef struct
{
	int rotationIncrements;
	SDL_Joystick * joystick;
	int joystickAxis;
} Wheel;
static unsigned int wheelCount = 0;
static Wheel * wheels = NULL;

static bool setWheel( unsigned int wheelIndex, SDL_Joystick * joystick, int joystickAxis )
{
	if( wheelIndex >= wheelCount )
	{
		unsigned int newWheelCount = wheelIndex + 1;
		Wheel * newWheels = (Wheel*) realloc( wheels, sizeof(Wheel) * newWheelCount );
		if( !newWheels )
			return false;
		memset( &newWheels[wheelCount], 0, sizeof(Wheel) * (newWheelCount - wheelCount) );
		wheels = newWheels;
		wheelCount = newWheelCount;
	}
	wheels[wheelIndex].joystick = joystick;
	wheels[wheelIndex].joystickAxis = joystickAxis;
	return true;
}


static SDL_Window * window = NULL;
static SDL_GLContext glContext = 0;

static bool init_sdl( int argc, const char ** argv )
{
	(void)argc;
	(void)argv;

	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
	{
		fprintf( stderr, "Failed to initialize SDL: %s\n", SDL_GetError() );
		return false;
	}

	for( int joystickIndex=0; joystickIndex<SDL_NumJoysticks(); joystickIndex++ )
	{
		SDL_Joystick * joystick = SDL_JoystickOpen( joystickIndex );
		if( joystick )
		{
			fprintf( stderr, "\nJoystick %d:\n", joystickIndex );
			fprintf( stderr, "\tName: \"%s\"\n", SDL_JoystickName( joystick ));
			fprintf( stderr, "\tAxes: %d\n", SDL_JoystickNumAxes( joystick ));
			SDL_JoystickClose( joystick );
		}
		else
		{
			fprintf( stderr, "Couldn't open Joystick %d - \"%s\": %s\n", joystickIndex, SDL_JoystickNameForIndex( joystickIndex ), SDL_GetError() );
		}
	}

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

	window = SDL_CreateWindow( "WheelChar SDL2/OpenGL Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
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
	glEnable( GL_DEPTH_TEST );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	return true;
}


static wcConfiguration * configuration = NULL;
static wcConnection * connection = NULL;

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

	return true;
}


static bool init( int argc, const char ** argv )
{
	if( !init_wc( argc, argv ) )
		return false;

	if( !init_sdl( argc, argv ) )
		return false;

	for( unsigned int wheelIndex=0; wheelIndex<wcConfiguration_getWheelCount( configuration ); wheelIndex++ )
	{
		setWheel( wheelIndex, NULL, 0 );
	}

	for( int i=1; i<argc; i++ )
	{
		if( !strcmp( argv[i], "--mapWheelToJoystickAxis" ) )
		{
			if( i < argc )
			{
				i++;
				char * tmp = (char*) malloc( strlen( argv[i] ) + 1 );
				strcpy( tmp, argv[i] );

				unsigned int wheelIndex=0;
				int joystickIndex=0;
				int axisIndex=0;
				char * token;
				token = strtok( tmp, ":" );
				if( token )
				{
					sscanf( token, "%u", &wheelIndex );
				}
				token = strtok( NULL, ":" );
				if( token )
				{
					sscanf( token, "%d", &joystickIndex );
				}
				token = strtok( NULL, ":" );
				if( token )
				{
					sscanf( token, "%d", &axisIndex );
				}

				if( wheelIndex >= wheelCount )
				{
					fprintf( stderr, "Cannot map wheel %d to joystick %d - wheel not configured\n", wheelIndex, joystickIndex );
				}
				else
				{
					SDL_Joystick * joystick = SDL_JoystickOpen( joystickIndex );
					if( !joystick )
					{
						fprintf( stderr, "Couldn't open Joystick %d - \"%s\": %s\n", joystickIndex, SDL_JoystickNameForIndex( joystickIndex ), SDL_GetError() );
					}
					else
					{
						setWheel( wheelIndex, joystick, axisIndex );
					}
				}

				free( tmp );
			}
			continue;
		}
	}

	fprintf( stderr, "\nEmulated Wheels:\n" );
	for( unsigned int wheelIndex=0; wheelIndex<wheelCount; wheelIndex++ )
	{
		fprintf( stderr, "\tWheel %d - mapped to joystick \"%s\" axis %d\n", wheelIndex, SDL_JoystickName( wheels[wheelIndex].joystick ), wheels[wheelIndex].joystickAxis );
	}

	return true;
}


static int mouseX = 0;
static int mouseY = 0;
static double delta = 0;

static void emulatedWheelMovement_update()
{
	static Uint64 lastTime = 0;
	Uint64 currentTime = SDL_GetPerformanceCounter();
	delta = (double)(currentTime - lastTime) / (double)SDL_GetPerformanceFrequency();
	lastTime = currentTime;
	SDL_GetRelativeMouseState( &mouseX, &mouseY );
}

static int emulatedWheelMovement_getIncrements( unsigned int i )
{
	int increments = 0.0;
	if( (i+1)&1 )
		increments += mouseX;
	if( (i+1)&2 )
		increments += mouseY * 3;

	if( i<wheelCount )
	{
		if( wheels[i].joystick )
		{
			double axis = (double)SDL_JoystickGetAxis( wheels[i].joystick, wheels[i].joystickAxis ) / 32767.0;
			if( axis > 1.0 )
				axis = 1.0;
			if( axis < -1.0 )
				axis = -1.0;
			increments += delta * axis * wcConfiguration_getWheelIncrementsPerTurn( configuration, i );
		}
	}

	return increments;
}


static void sendWheelMovement( unsigned int index, int movementIncrements )
{
	wcPacket packet;
	wcPacket_Wheel_create( (wcPacket_Wheel*)&packet, index, WC_ERROR_NOERROR, movementIncrements );
	wcConnection_write( connection, &packet );
}


// http://stackoverflow.com/questions/12089514/real-modulo-operator-in-c-c
static inline int modulo( int a, int b )
{
	const int result = a % b;
	return result >= 0 ? result : result + b;
}


static void update()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	emulatedWheelMovement_update();

	for( unsigned int i=0; i<wheelCount; i++ )
	{
		int increments = emulatedWheelMovement_getIncrements( i );
		sendWheelMovement( i, increments );
		wheels[i].rotationIncrements += increments;
		wheels[i].rotationIncrements = modulo( wheels[i].rotationIncrements, wcConfiguration_getWheelIncrementsPerTurn( configuration, i ) );
		glPushMatrix();
			unsigned char color = i+1;
			glColor3f( color&2, color&1, color&4 );
			glRotatef( ( (double)wheels[i].rotationIncrements / (double)wcConfiguration_getWheelIncrementsPerTurn( configuration, i ) ) * 360.0f, 0,0,1 );
			glBegin( GL_QUADS );
				glVertex3f( -0.5f, -0.5f, i*0.1f );
				glVertex3f( 0.5f, -0.5f, i*0.1f );
				glVertex3f( 0.5f, 0.5f, i*0.1f );
				glVertex3f( -0.5f, 0.5f, i*0.1f );
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
	wcConnection_close( connection );
	connection = NULL;
	wcConfiguration_delete( configuration );
	configuration = NULL;
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
