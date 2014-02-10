#ifndef _WC_PACKET_H_
#define _WC_PACKET_H_

#include "wc_export.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * \defgroup wcPacket Packet protocol
 * \brief The protocol used to communicate with the controller.
 *
 * Example:
\code{.c}
...
wcPacket packet;
wcPacket_RequestInfo_create( (wcPacket_RequestInfo*)&packet );
wcConnection_write( connection, &packet );
...
\endcode
 * @{
 */

/// The maximum size of a packet.
#define WC_PACKET_MAXSIZE 255
/// The maximum size of a packet's payload data.
#define WC_PACKET_MAXLENGTH ( WC_PACKET_MAXSIZE - sizeof(wcPacket_Header) )

/// A message - transports a simple string.
#define WC_PACKET_MESSAGE_TYPE      0
/// Maximum length of a message packet.
#define WC_PACKET_MESSAGE_MAXLENGTH ( WC_PACKET_MAXSIZE - sizeof(wcPacket_Header) )

/// A wheel packet.
#define WC_PACKET_WHEEL_TYPE        1
/// The length of a wheel packet.
#define WC_PACKET_WHEEL_LENGTH      ( sizeof(wcPacket_Wheel) - sizeof(wcPacket_Header) )

/// Requests an information message from receiver.
#define WC_PACKET_REQUESTINFO_TYPE  2


#pragma pack(push,1)

/// The header used in each packet.
typedef struct
{
	uint8_t type;   ///< One of the values WC_PACKET_*_TYPE .
	uint8_t length; ///< The number of payload bytes following this header.
} wcPacket_Header;

/// Generic packet.
typedef struct
{
	wcPacket_Header header;             ///< The header.
	uint8_t _data[WC_PACKET_MAXLENGTH]; ///< Maximum allowed payload data length.
} wcPacket;


/// Requests an information message from receiver.
typedef struct
{
	wcPacket_Header header; ///< The header.
} wcPacket_RequestInfo;


/// A message - transports a simple string.
typedef struct
{
	wcPacket_Header header;                 ///< The header.
	char text[WC_PACKET_MESSAGE_MAXLENGTH]; ///< Maximum possible message length.
} wcPacket_Message;


/// A wheel packet.
typedef struct
{
	wcPacket_Header header; ///< The header.
	uint8_t channel;        ///< The channel index.
	uint8_t error;          ///< A value defined in \ref wcErrorTypes .
	int16_t value;          ///< The number of increments accumulated since last wheel packet.
} wcPacket_Wheel;


#pragma pack(pop)


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Checks a received packet for validity.
 *
 * Checks the packet's header for validity.
 * \param packet The pointer to the received packet.
 * \param size The number of bytes received.
 * \return True if packet is valid.
 */
WC_EXPORT bool wcPacket_isValid( const wcPacket * packet, uint16_t size );

/**
 * \brief Calculates the size of a packet.
 *
 * \param packet The pointer to a packet.
 * \return The packet's size.
 */
WC_EXPORT uint16_t wcPacket_size( const wcPacket * packet );

/**
 * \brief Initializes a message type packet.
 *
 * \param message The pointer to a packet.
 * \param string A null terminated message string.
 */
WC_EXPORT void wcPacket_Message_create( wcPacket_Message * message, const char * string );

/**
 * \brief Initializes a wheel type packet.
 *
 * \param wheel The pointer to a packet.
 * \param channel The wheels channel index.
 * \param error Error flags.
 * \param value The signed number of increments the wheel moved.
 */
WC_EXPORT void wcPacket_Wheel_create( wcPacket_Wheel * wheel, uint8_t channel, uint8_t error, int16_t value );

/**
 * \brief Initializes a requestInfo type packet.
 *
 * \param requestInfo The pointer to a packet.
 */
WC_EXPORT void wcPacket_RequestInfo_create( wcPacket_RequestInfo * requestInfo );

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
