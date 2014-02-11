#ifndef _WC_WHEELMOVEMENT_PACKET_H_
#define _WC_WHEELMOVEMENT_PACKET_H_


#include "wc_export.h"
#include <wc/WheelMovement.h>
#include <wc/Packet.h>

/**
 * \defgroup wcWheelMovement_Packet Wheel movement from packet
 * \brief Handling wheel movements from packets.
 * @{
 */

static inline void wcWheelMovement_createFromPacket( wcWheelMovement * wm, const wcPacket_Wheel * pw )
{
	wcWheelMovement_create( wm, pw->channel );
	wcWheelMovement_accumulate( wm, pw->value, pw->error );
}


static inline void wcWheelMovement_accumulateFromPacket( wcWheelMovement * wm, const wcPacket_Wheel * pw )
{
	wcWheelMovement_accumulate( wm, pw->value, pw->error );
}

/**
 * @}
 */

#endif
