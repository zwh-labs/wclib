#ifndef _WC_CONFIGURATION_H_
#define _WC_CONFIGURATION_H_


#include "wc_export.h"
#include <stdbool.h>

/**
 * \defgroup wcConfiguration Configuration
 * \brief Controller configuration
 *
 * The configuration is used to specify:
 *  - how to connect to a controller
 *  - how many wheels there are
 *  - and how each wheel is configured.
 *
 * Example usage:
\code{.c}
wcConfiguration * configuration = wcConfiguration_new();
wcConfiguration_setDevicePath( configuration, "/dev/pts/4" );
wcConfiguration_setWheel( configuration, 0, 4096 );
wcConfiguration_setWheel( configuration, 1, 4096 );
...
wcConfiguration_delete( configuration );
\endcode
 * This would define a controller connected to "/dev/pts/4" with 2 wheels attached each with 4096 increments per turn.
 * @{
 */

/**
 * \brief The configuration structure.
 */
typedef struct _wcConfiguration wcConfiguration;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Creates a new configuration.
 *
 * Allocates a new configuration on the heap and returns it's pointer.
 * \return A pointer to a new configuration.
 */
WC_EXPORT wcConfiguration * wcConfiguration_new();

/**
 * \brief Deletes a configuration.
 *
 * Deletes a configuration previously created by wcConfiguration_new() .
 * \param configuration The pointer to a configuration.
 * \return True if configuration could be deleted.
 */
WC_EXPORT bool wcConfiguration_delete( wcConfiguration * configuration );

/**
 * \brief Returns the device path.
 *
 * To set the path use wcConfiguration_setDevicePath( wcConfiguration * configuration, const char * devicePath ) .
 * \param configuration The pointer to a configuration.
 * \return A null-terminated string or a null-pointer if not set.
 */
WC_EXPORT const char * wcConfiguration_getDevicePath( const wcConfiguration * configuration );

/**
 * \brief Returns the number of configured wheels.
 *
 * This will return the highest index used in a call to wcConfiguration_setWheel( wcConfiguration * configuration, unsigned int wheelIndex, unsigned int incrementsPerTurn ) + 1.
 * \param configuration The pointer to a configuration.
 * \return The number of configured wheels.
 */
WC_EXPORT unsigned int wcConfiguration_getWheelCount( const wcConfiguration * configuration );

/**
 * \brief Returns the number of increments per turn of a wheel.
 *
 * Returns the number of increments per turn for the given wheel's index previously set by wcConfiguration_setWheel( wcConfiguration * configuration, unsigned int wheelIndex, unsigned int incrementsPerTurn ) .
 * \param configuration The pointer to a configuration.
 * \param wheelIndex Index of the wheel.
 * \return The number of increments per turn for the given wheel's index.
 */
WC_EXPORT unsigned int wcConfiguration_getWheelIncrementsPerTurn( const wcConfiguration * configuration, unsigned int wheelIndex );

/**
 * \brief Sets the device path
 *
 * This is a platform specific string identifying a path to the controller's device.\n
 * On Linux this might for example be "/dev/ttyACM0" - or "COM1" on Windows.\n
 * \param configuration The pointer to a configuration.
 * \param devicePath The path to the controller's device.
 */
WC_EXPORT void wcConfiguration_setDevicePath( wcConfiguration * configuration, const char * devicePath );

/**
 * \brief Sets the configuration of a wheel.
 *
 * Each wheel's incremental rotary encoder generates a fixed number of increments for a full turn.\n
 * Set number of increments for each wheel using this function.\n
 * \note This function will allocate memory for each wheel's index up to the given index if it is not yet configured.
 * \param configuration The pointer to a configuration.
 * \param wheelIndex Index of the wheel.
 * \param incrementsPerTurn The number of increments per turn for this wheel.
 * \return True on success.
 */
WC_EXPORT bool wcConfiguration_setWheel( wcConfiguration * configuration, unsigned int wheelIndex, unsigned int incrementsPerTurn );

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
