#ifndef _WC_THREAD_H_
#define _WC_THREAD_H_


#include "wc_export.h"
#include <wc/Connection.h>
#include <wc/WheelMovement.h>

#include <stdbool.h>

/**
 * \defgroup wcThread Thread
 * \brief Connection thread
 * @{
 */

/**
 * \brief The thread structure.
 */
typedef struct _wcThread wcThread;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Starts a new thread listening on a connection.
 *
 * \param connection The connection to listen on.
 * \return A pointer to the created thread or NULL on error.
 */
WC_EXPORT wcThread * wcThread_start( wcConnection * connection );

/**
 * \brief Stops a thread.
 *
 * \param thread The thread to stop.
 * \return True on success.
 */
WC_EXPORT bool wcThread_stop( wcThread * thread );

/**
 * \brief Returns if the thread is still running.
 *
 * \param thread The thread.
 * \return True if the thread is still running.
 */
WC_EXPORT bool wcThread_isRunning( wcThread * thread );

/**
 * \brief Fetches the accumulated wheel movements.
 *
 * Returns the accumulated wheel movements for the given wheel index since the last call using the same index.
 * \param thread The thread.
 * \param index The wheel's index to retrieve.
 * \return The accumulated wheel movements.
 */
WC_EXPORT wcWheelMovement wcThread_retrieveWheelMovement( wcThread * thread, unsigned int index );

/**
 * \brief Returns the number of tracked wheels.
 *
 * \param thread The thread.
 * \return The number of tracked wheels.
 */
WC_EXPORT unsigned int wcThread_getWheelCount( wcThread * thread );

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
