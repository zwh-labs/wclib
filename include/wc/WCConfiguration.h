#ifndef _WCCONFIGURATION_H_
#define _WCCONFIGURATION_H_


#include <stdio.h>
#include <stdbool.h>


typedef struct _WCConfiguration WCConfiguration;


const char * WCConfiguration_getDevicePath( const WCConfiguration * configuration );
unsigned int WCConfiguration_getWheelCount( const WCConfiguration * configuration );
unsigned int WCConfiguration_getWheelIncrementsPerTurn( const WCConfiguration * configuration, unsigned int wheelIndex );

void WCConfiguration_setDevicePath( WCConfiguration * configuration, const char * devicePath );
bool WCConfiguration_setWheel( WCConfiguration * configuration, unsigned int wheelIndex, unsigned int incrementsPerTurn );

bool WCConfiguration_delete( WCConfiguration * configuration );
WCConfiguration * WCConfiguration_new();

int WCConfiguration_fprint( FILE * stream, const WCConfiguration * configuration );
int WCConfiguration_print( const WCConfiguration * configuration );


#endif
