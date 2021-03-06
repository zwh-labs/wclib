#include <wc/Configuration.h>

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#define DEFAULT_WHEELINCREMENTSPERTURN (4096)

#ifdef _WIN32
	#define DEFAULT_DEVICEPATH "\\\\.\\COM1"
#else
	#define DEFAULT_DEVICEPATH "/dev/ttyACM0"
#endif


typedef struct _wcConfigurationWheel wcConfigurationWheel;
struct _wcConfigurationWheel
{
	unsigned int incrementsPerTurn;
};


struct _wcConfiguration
{
	char * devicePath;
	unsigned int wheelCount;
	wcConfigurationWheel * wheelConfigurations;
};


const char * wcConfiguration_getDevicePath( const wcConfiguration * configuration )
{
	return configuration->devicePath;
}


unsigned int wcConfiguration_getWheelCount( const wcConfiguration * configuration )
{
	return configuration->wheelCount;
}


unsigned int wcConfiguration_getWheelIncrementsPerTurn( const wcConfiguration * configuration, unsigned int wheelIndex )
{
	if( wheelIndex >= configuration->wheelCount )
		return DEFAULT_WHEELINCREMENTSPERTURN;
	return configuration->wheelConfigurations[wheelIndex].incrementsPerTurn;
}


void wcConfiguration_setDevicePath( wcConfiguration * configuration, const char * devicePath )
{
	free( configuration->devicePath );
	configuration->devicePath = (char*) malloc( strlen(devicePath) + 1 );
	strcpy( configuration->devicePath, devicePath );
}


bool wcConfiguration_setWheel( wcConfiguration * configuration, unsigned int wheelIndex, unsigned int incrementsPerTurn )
{
	if( wheelIndex >= configuration->wheelCount )
	{
		// resize wheel configuration
		unsigned int newWheelCount = wheelIndex + 1;
		wcConfigurationWheel * newWheelConfigurations = (wcConfigurationWheel*) realloc( configuration->wheelConfigurations, sizeof(wcConfigurationWheel) * newWheelCount );
		if( !newWheelConfigurations )
			return false;
		// zero all fields for new wheel configurations
		memset( &newWheelConfigurations[configuration->wheelCount], 0, sizeof(wcConfigurationWheel) * ( newWheelCount - configuration->wheelCount ) );
		// set some default values for undefined wheels - happens when indexing a wheel with a gap to the last defined wheel
		for( unsigned int i = configuration->wheelCount; i < wheelIndex; i++ )
		{
			newWheelConfigurations[i].incrementsPerTurn = DEFAULT_WHEELINCREMENTSPERTURN;
		}
		// update configuration
		configuration->wheelConfigurations = newWheelConfigurations;
		configuration->wheelCount = newWheelCount;
	}
	// set the wheel configuration for indexed wheel
	configuration->wheelConfigurations[wheelIndex].incrementsPerTurn = incrementsPerTurn;
	return true;
}


bool wcConfiguration_delete( wcConfiguration * configuration )
{
	if( !configuration )
		return false;
	free( configuration->devicePath );
	free( configuration->wheelConfigurations );
	free( configuration );
	return true;
}


wcConfiguration * wcConfiguration_new()
{
	wcConfiguration * configuration = (wcConfiguration*) malloc( sizeof(wcConfiguration) );
	memset( configuration, 0, sizeof(wcConfiguration) );

	wcConfiguration_setDevicePath( configuration, DEFAULT_DEVICEPATH );
	configuration->wheelCount = 0;

	return configuration;
}
