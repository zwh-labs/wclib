#include <wc/Connection.h>
#include <wc/Packet.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


struct _wcConnection
{
	int fd;
	struct termios initialTio;
	struct termios tio;
};


wcConnection * wcConnection_open( const char * devicePath )
{
	wcConnection * connection = (wcConnection*) malloc( sizeof(wcConnection) );

	connection->fd = open( devicePath, O_RDWR | O_NOCTTY );
	if( connection->fd < 0 )
	{
		free( connection );
		return NULL;
	}

	tcgetattr( connection->fd, &(connection->initialTio) );

	memset( &(connection->tio), 0, sizeof(connection->tio) );
	connection->tio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	connection->tio.c_iflag = IGNPAR;
	connection->tio.c_oflag = 0;
	connection->tio.c_lflag = 0;
	connection->tio.c_cc[VTIME] = 10;
	connection->tio.c_cc[VMIN] = 0;
	tcflush( connection->fd, TCIFLUSH );
	tcsetattr( connection->fd, TCSANOW, &connection->tio );

	return connection;
}


bool wcConnection_close( wcConnection * connection )
{
	if( !connection )
		return false;
	tcsetattr( connection->fd, TCSANOW, &(connection->initialTio) );
	int ret = close( connection->fd );
	free( connection );
	return ret == 0;
}


// just a wrapper that won't return until "length" bytes are read or an error occurs - be sure to use a blocking "fd"
static ssize_t readBytes( int fd, void * buffer, size_t length )
{
	size_t bytesRead = 0;
	while( bytesRead < length )
	{
		ssize_t ret = read( fd, ((char*)buffer) + bytesRead, length - bytesRead );
		if( ret < 0 && errno == EINTR )
			continue;
		if( ret <= 0 )
			return ret;
		bytesRead += ret;
	}
	return bytesRead;
}


int wcConnection_read( wcConnection * connection, wcPacket * packet )
{
	int ret;

	// header
	ret = readBytes( connection->fd, &(packet->header), sizeof(wcPacket_Header) );
	if( ret != sizeof(wcPacket_Header) )
		return ret; // return error or size read

	// remaining packet
	ret = readBytes( connection->fd, packet->_data, packet->header.length );
	if( ret < 0 )
		return ret; // return error

	return ret + sizeof(wcPacket_Header); // return size read + size of header
}


int wcConnection_write( wcConnection * connection, const wcPacket * packet )
{
	return write( connection->fd, packet, wcPacket_size( packet ) );
}
