#ifndef _WC_CONNECTION_H_
#define _WC_CONNECTION_H_


#include "wc_export.h"
#include <wc/Packet.h>

#include <stdbool.h>

/**
 * \defgroup wcConnection Connection
 * \brief Handles the connection to a controller.
 *
 * Example usage:
\code{.c}
wcConnection * connection = wcConnection_open( "/dev/pts/4" );
...
wcConnection_close( connection );
\endcode
 * @{
 */

/**
 * \brief The connection structure.
 */
typedef struct _wcConnection wcConnection;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Opens a new connection.
 *
 * \param devicePath The path to the controller's device.
 * \return A pointer to a new connection.
 */
WC_EXPORT wcConnection * wcConnection_open( const char * devicePath );

/**
 * \brief Closes an opened connection
 *
 * \param controller The used connection.
 * \return True if connection could be closed.
 */
WC_EXPORT bool wcConnection_close( wcConnection * controller );

/**
 * \brief Reads a packet from an opened connection
 *
 * \param controller The used connection.
 * \param packet A pointer to a \ref WC_PACKET_MAXSIZE bytes long buffer.
 * \return The number of bytes read. Negative on error. If the connection timed out 0 is returned.
 */
WC_EXPORT int wcConnection_read( wcConnection * controller, wcPacket * packet );

/**
 * \brief Writes a packet to an opened connection
 *
 * \param controller The used connection.
 * \param packet A pointer to a packet.
 * \return The number of bytes written. Negative on error.
 */
WC_EXPORT int wcConnection_write( wcConnection * controller, const wcPacket * packet );

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
