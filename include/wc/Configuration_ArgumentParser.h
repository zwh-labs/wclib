#ifndef _WC_CONFIGURATION_ARGUMENTPARSER_H_
#define _WC_CONFIGURATION_ARGUMENTPARSER_H_


#include "wc_export.h"
#include <wc/Configuration.h>

/**
 * \defgroup wcConfiguration_ArgumentParser Configuration argument parser
 * \brief Reads configuration from command line arguments.
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Creates a new configuration based on command line arguments.
 *
 * After usage, free the resources allocated by this call using wcConfiguration_delete( wcConfiguration * configuration ) on the returned configuration pointer.
 * \return A pointer to a new configuration.
 */
WC_EXPORT wcConfiguration * wcConfiguration_newFromArguments( int argc, const char ** argv );

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
