#ifndef _WC_WHEELMOVEMENT_CONFIGURATION_H_
#define _WC_WHEELMOVEMENT_CONFIGURATION_H_


#include "wc_export.h"
#include <wc/WheelMovement.h>
#include <wc/Configuration.h>

/**
 * \defgroup wcWheelMovement_Configuration Wheel movement configuration utilities
 * \brief Configuration specific wheel movement utility functions.
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Return wheel movement as fraction of a full turn.
 *
 * \param wm Wheel movement pointer.
 * \param c Pointer to configuration.
 * \return A pointer to a new connection.
 */
WC_EXPORT double wcWheelMovement_getTurns( const wcWheelMovement * wm, const wcConfiguration * c );

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
