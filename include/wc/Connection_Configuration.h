#ifndef _WC_CONNECTION_CONFIGURATION_H_
#define _WC_CONNECTION_CONFIGURATION_H_


#include "wc_export.h"
#include <wc/Connection.h>
#include <wc/Configuration.h>

/**
 * \defgroup wcConnection_Configuration Connection from configuration
 * \brief Opens a connection as defined in a configuration.
 *
 * Example usage:
\code{.c}
...
wcConnection * connection = wcConnection_openFromConfiguration( configuration );
...
wcConnection_close( connection );
\endcode
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Opens a new connection.
 *
 * \param configuration The controllers configuration.
 * \return A pointer to a new connection.
 */
WC_EXPORT wcConnection * wcConnection_openFromConfiguration( const wcConfiguration * configuration );

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
