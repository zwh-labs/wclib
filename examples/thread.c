#define _POSIX_SOURCE

#include <wc/Configuration.h>
#include <wc/Configuration_stdio.h>
#include <wc/Configuration_ArgumentParser.h>
#include <wc/Connection.h>
#include <wc/Connection_Configuration.h>
#include <wc/Thread.h>
#include <wc/WheelMovement.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


static volatile bool quit = false;


#ifdef _WIN32
	#include <windows.h>
	static void sleep( int sec )
	{
		Sleep( sec * 1000 );
	}
	static BOOL quitHandler( DWORD fdwCtrlType )
	{
		if( fdwCtrlType == CTRL_C_EVENT )
		{
			quit = true;
			return TRUE;
		}
		return FALSE;
	}
	static void setupQuitOnSIGINT()
	{
		SetConsoleCtrlHandler( (PHANDLER_ROUTINE) quitHandler, TRUE );
	}
#else
	#include <unistd.h>
	#include <signal.h>
	static void quitHandler( int s )
	{
		(void) s;
		quit = true;
	}
	static void setupQuitOnSIGINT()
	{
		struct sigaction sigIntHandler;

		sigIntHandler.sa_handler = quitHandler;
		sigemptyset(&sigIntHandler.sa_mask);
		sigIntHandler.sa_flags = 0;

		sigaction(SIGINT, &sigIntHandler, NULL);
	}
#endif


int main( int argc, const char ** argv )
{
	fprintf( stderr, "Reading configuration ... " ); fflush( stderr );
	wcConfiguration * configuration = wcConfiguration_newFromArguments( argc, argv );
	if( !configuration )
		goto configuration_failed;
	fprintf( stderr, "done\n" );
	wcConfiguration_fprint( stderr, configuration );

	fprintf( stderr, "Opening connection ... " ); fflush( stderr );
	wcConnection * connection = wcConnection_openFromConfiguration( configuration );
	if( !connection )
		goto connection_failed;
	fprintf( stderr, "done\n" );

	fprintf( stderr, "Starting thread on connection ... " ); fflush( stderr );
	wcThread * thread = wcThread_start( connection );
	if( !thread )
		goto thread_failed;
	fprintf( stderr, "done\n" );

	setupQuitOnSIGINT();
	fprintf( stderr, "Polling thread each second - Press Ctrl-C to shutdown\n" );
	while( !quit )
	{
		for( unsigned int i = 0; i<wcThread_getWheelCount( thread ); i++ )
		{
			wcWheelMovement wm = wcThread_retrieveWheelMovement( thread, i );
			printf( "Wheel:\tchannel=%d\terror=%d\tvalue=%d\n", i, wcWheelMovement_getError( &wm ), wcWheelMovement_getIncrements( &wm ) );
		}
		sleep( 1 );
	}

	fprintf( stderr, "Stopping thread ... " ); fflush( stderr );
	if( !wcThread_stop( thread ) )
		goto thread_failed;
	fprintf( stderr, "done\n" );

	fprintf( stderr, "Closing connection ... " ); fflush( stderr );
	if( !wcConnection_close( connection ) )
		goto connection_failed;
	fprintf( stderr, "done\n" );

	wcConfiguration_delete( configuration );
	return 0;

thread_failed:
	wcConnection_close( connection );
connection_failed:
	wcConfiguration_delete( configuration );
configuration_failed:
	fprintf( stderr, "failed\n" );
	return 1;
}
