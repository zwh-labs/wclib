#ifndef _WC_WHEELMOVEMENT_H_
#define _WC_WHEELMOVEMENT_H_


#include <wc/Error.h>


/**
 * \defgroup wcWheelMovement Wheel movement
 * \brief A structure representing wheel movements.
 * @{
 */

/// A wheel movement.
typedef struct
{
	unsigned int index; ///< Channel index.
	int increments;     ///< The number of increments moved.
	wcError error;      ///< Error indicator.
} wcWheelMovement;


#ifdef _MSC_VER // C99 anyone?
	#define inline __forceinline
#endif


static inline unsigned int wcWheelMovement_getIndex( const wcWheelMovement * wm )
	{ return wm->index; }

static inline int wcWheelMovement_getIncrements( const wcWheelMovement * wm )
	{ return wm->increments; }

static inline wcError wcWheelMovement_getError( const wcWheelMovement * wm )
	{ return wm->error; }


static inline void wcWheelMovement_create( wcWheelMovement * wm, unsigned int index )
	{ wm->index = index; wm->increments = 0; wm->error = WC_ERROR_NOERROR; }

static inline void wcWheelMovement_reset( wcWheelMovement * wm )
	{ wm->increments = 0; wm->error = WC_ERROR_NOERROR; }

static inline void wcWheelMovement_accumulate( wcWheelMovement * wm, int increment, wcError error )
	{ wm->increments += increment; wm->error |= error; }

/**
 * @}
 */

#endif
