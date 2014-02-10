#ifndef _WC_WHEELPIDCONTROLLER_H_
#define _WC_WHEELPIDCONTROLLER_H_


#include "wc_export.h"
#include <wc/Configuration.h>
#include <stdbool.h>

/**
 * \defgroup wcWheelPIDController Wheel PID controller
 * \brief Implements a simple PID controller.
 *
 * This module can be used if the users application does not provide any mean to set rotations directly.\n
 * For example in a physics simulation only a torque may be applied to a simulated wheel.\n
 * To move the wheel according to the wheel signals of the incremental rotary encoder connected to the controller,
 * the application must track both absolute wheel angles - The accumulated increments of the real wheel and the simulated wheel's angle expressed as increments.\n
 * Both values can then be used as input for the PID controller (in this case wcWheelPIDController_updateAngular( wcWheelPIDController *, int, int, unsigned int, double ) ).
 * The output (the return value of the update function) can be applied as a torque to the simulated wheel.
 *
 * @{
 */

typedef struct _wcWheelPIDController wcWheelPIDController;

#ifdef __cplusplus
extern "C" {
#endif

WC_EXPORT wcWheelPIDController * wcWheelPIDController_new();
WC_EXPORT bool wcWheelPIDController_delete( wcWheelPIDController * controller );

WC_EXPORT void wcWheelPIDController_reset( wcWheelPIDController * controller );

WC_EXPORT void wcWheelPIDController_setProportionalGain( wcWheelPIDController * controller, double gain );
WC_EXPORT void wcWheelPIDController_setIntegralGain( wcWheelPIDController * controller, double gain );
WC_EXPORT void wcWheelPIDController_setDerivativeGain( wcWheelPIDController * controller, double gain );
WC_EXPORT void wcWheelPIDController_setWindupGuard( wcWheelPIDController * controller, double guard );

WC_EXPORT double wcWheelPIDController_getProportionalGain( const wcWheelPIDController * controller );
WC_EXPORT double wcWheelPIDController_getIntegralGain( const wcWheelPIDController * controller );
WC_EXPORT double wcWheelPIDController_getDerivativeGain( const wcWheelPIDController * controller );
WC_EXPORT double wcWheelPIDController_getWindupGuard( const wcWheelPIDController * controller );

WC_EXPORT double wcWheelPIDController_update( wcWheelPIDController * controller, double currentError, double delta );
WC_EXPORT double wcWheelPIDController_updateAngular( wcWheelPIDController * controller, int targetAngleIncrements, int actualAngleIncrements, unsigned int incrementsPerTurn, double delta );

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
