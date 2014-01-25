#ifndef _LNCV_DEFAULT_STORAGE_H__
#define _LNCV_DEFAULT_STORAGE_H__

#include <LocoNet.h>

#define VARIABLE_NAME_ARTNR artNr
#define _LNCV_TYPE_ARTNR const prog_uint16_t 
#define _LNCV_VARIABLE_ARTNR _LNCV_TYPE_ARTNR VARIABLE_NAME_ARTNR PROGMEM

#define SET_ARTNT(value) _LNCV_VARIABLE_ARTNR = (value);

extern _LNCV_VARIABLE_ARTNR;

uint16_t readModuleAddress();

/**
 * Notification that an LNCVDiscover message was sent. If a module wants to react to this,
 * It should return LNCV_LACK_OK and set ArtNr and ModuleAddress accordingly.
 * A response just as in the case of notifyLNCVProgrammingStart will be generated.
 * If a module responds to a LNCVDiscover, it should apparently enter programming mode immediately.
 */
int8_t notifyLNCVdiscover( uint16_t & ArtNr, uint16_t & ModuleAddress );

/**
 * Notification that a LNCVProgrammingStart message was received. Application code should process this message and
 * set the return code to LNCV_LACK_OK in case this message was intended for this module (i.e., the addresses match).
 * In case ArtNr and/or ModuleAddress were Broadcast addresses, the Application Code should replace them by their
 * real values.
 * The calling code will then generate an appropriate ACK message.
 * A return code different than LACK_LNCV_OK will result in no response being sent.
 */
int8_t notifyLNCVprogrammingStart ( uint16_t & ArtNr, uint16_t & ModuleAddress );

/**
 * Notification that a LNCV read request message was received. Application code should process this message,
 * set the lncvValue to its respective value and set an appropriate return code.
 * return LNCV_LACK_OK leads the calling code to create a response containing lncvValue.
 * return code >= 0 leads to a NACK being sent.
 * return code < 0 will result in no reaction.
 */
int8_t notifyLNCVread ( uint16_t ArtNr, uint16_t lncvAddress, uint16_t, uint16_t & lncvValue );

/**
 * Notification that a LNCV value should be written. Application code should process this message and
 * set an appropriate return code.
 * Note 1: LNCV 0 is spec'd to be the ModuleAddress.
 * Note 2: Changes to LNCV 0 must be reflected IMMEDIATELY! E.g. the programmingStop command will
 * be sent using the new address.
 *
 * return codes >= 0 will result in a LACK containing the return code being sent.
 * return codes < 0 will result in no reaction.
 */
int8_t notifyLNCVwrite ( uint16_t ArtNr, uint16_t lncvAddress, uint16_t lncvValue );

/**
 * Notification that an LNCV Programming Stop message was received.
 * This message is noch ACKed, thus does not require a result to be returned from the application.
 */
void notifyLNCVprogrammingStop( uint16_t ArtNr, uint16_t ModuleAddress );

// System Variable notify Call-back functions
extern void notifySVChanged(uint16_t Offset) __attribute__ ((weak));

#endif