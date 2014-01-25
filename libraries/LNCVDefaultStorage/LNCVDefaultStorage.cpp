#include <LNCVDefaultStorage.h>
#include <avr/eeprom.h>

bool programmingModeActive = false;

#undef DO_DEBUG

#define LNCV_SPECIAL_VALUE_OFFSET (2)
#define LNCV_ADDR_MODULEADDR (0)

#define LOAD_ARTNR pgm_read_word(VARIABLE_NAME_ARTNR);

uint16_t readModuleAddress() {
	uint16_t result = eeprom_read_byte((uint8_t*)(SV_ADDR_NODE_ID_H - LNCV_SPECIAL_VALUE_OFFSET));
	result <<= 8;
	result |= eeprom_read_byte((uint8_t*)(SV_ADDR_NODE_ID_L - LNCV_SPECIAL_VALUE_OFFSET));
	return result;
}

int8_t notifyLNCVprogrammingStart( uint16_t & ArtNr, uint16_t & ModuleAddress ) {
  #ifdef DO_DEBUG
  Serial.print(F("notifyLNCVProgrammingStart "));
  Serial.print(ArtNr);
  Serial.print(" ");
  Serial.print(ModuleAddress);
  Serial.print("\n");
  #endif
  // Enter programming mode. If we already are in programming mode,
  // we simply send a response and nothing else happens.
  uint16_t myArtNr = LOAD_ARTNR;
  if (ArtNr == myArtNr && (ModuleAddress == readModuleAddress() || ModuleAddress == 0xFFFF)) {
    programmingModeActive = true;
    if (ModuleAddress == 0xFFFF) {
      ModuleAddress = readModuleAddress();
    }
    return LNCV_LACK_OK;
  } else {
    return LNCV_LACK_ERROR_GENERIC;
  }
}

int8_t notifyLNCVread( uint16_t ArtNr, uint16_t lncvAddress, uint16_t, uint16_t & lncvValue ) {
	#ifdef DO_DEBUG
	Serial.print(F("notifyLNCVread\n"));
	#endif
	// Behavior mostly cloned from LocoNetSystemVariableClass::readSVStorage()
	uint16_t myArtNr = LOAD_ARTNR;
	if ( programmingModeActive && ArtNr == myArtNr ) {
		if  ( lncvAddress == LNCV_ADDR_MODULEADDR) {
			// Address 0 is *always* the module address
			lncvValue = readModuleAddress();
			return LNCV_LACK_OK;
		} else if ( lncvAddress == SV_ADDR_EEPROM_SIZE) { // This is address 1
#if (E2END==0x0FF)	/* E2END is defined in processor include */
									lncvValue = SV_EE_SZ_256;
#elif (E2END==0x1FF)
									lncvValue = SV_EE_SZ_512;
#elif (E2END==0x3FF)
									lncvValue = SV_EE_SZ_1024;
#elif (E2END==0x7FF)
									lncvValue = SV_EE_SZ_2048;
#elif (E2END==0xFFF)
									lncvValue = SV_EE_SZ_4096;
#else
									lncvValue = 0xFF;
#endif
			return LNCV_LACK_OK;
		} else if ( lncvAddress == SV_ADDR_SW_VERSION ) { // This is address 2
			//lncvValue = swVersion ;
			//return LNCV_LACK_OK;
			return LNCV_LACK_ERROR_UNSUPPORTED;
		} else if ( lncvAddress - LNCV_SPECIAL_VALUE_OFFSET <= E2END) {
			lncvAddress -= LNCV_SPECIAL_VALUE_OFFSET;    // Map SV Address to EEPROM Offset - Skip SV_ADDR_EEPROM_SIZE & SV_ADDR_SW_VERSION
			lncvValue = eeprom_read_byte((uint8_t*)lncvAddress);
			return LNCV_LACK_OK;
		} else {
			// Not a valid lncvAddress
			return LNCV_LACK_ERROR_UNSUPPORTED;
		}
	} else {
		return -1;
	}
}

/**
 * Notifies the code on the reception of a write request
 * Changes are applied immediately, but only persist across a reset
 * if the programming sequence is prroperly ended.
 */
int8_t notifyLNCVwrite( uint16_t ArtNr, uint16_t lncvAddress, uint16_t lncvValue ) {
	#ifdef DO_DEBUG
	Serial.print(F("notifyLNCVwrite\n"));
	#endif
	uint16_t myArtNr = LOAD_ARTNR;
	if (programmingModeActive && ArtNr == myArtNr) {
	  
		if (lncvAddress == LNCV_ADDR_MODULEADDR) {
			// Change to module address
			uint8_t oldValue = eeprom_read_byte((uint8_t*)(SV_ADDR_NODE_ID_L - LNCV_SPECIAL_VALUE_OFFSET));
			if (((uint8_t)lncvValue) != oldValue) {
				eeprom_write_byte((uint8_t*)(SV_ADDR_NODE_ID_L - LNCV_SPECIAL_VALUE_OFFSET), ((uint8_t)lncvValue));
			}
			oldValue = eeprom_read_byte((uint8_t*)(SV_ADDR_NODE_ID_H - LNCV_SPECIAL_VALUE_OFFSET));
			if (((uint8_t)(lncvValue >> 8)) != oldValue) {
				eeprom_write_byte((uint8_t*)(SV_ADDR_NODE_ID_H - LNCV_SPECIAL_VALUE_OFFSET), ((uint8_t)(lncvValue >> 8)));
			}
			if(notifySVChanged) {
				notifySVChanged(lncvAddress);
			}
			return LNCV_LACK_OK;
		} else if (lncvAddress == SV_ADDR_EEPROM_SIZE || lncvAddress == SV_ADDR_SW_VERSION) {
			return LNCV_LACK_ERROR_READONLY;
		} else if (lncvAddress - LNCV_SPECIAL_VALUE_OFFSET > E2END) {
			return LNCV_LACK_ERROR_UNSUPPORTED;
		} else if (lncvValue > 0xFF) {
			return LNCV_LACK_ERROR_OUTOFRANGE;
		} else {
			lncvAddress -= LNCV_SPECIAL_VALUE_OFFSET;      // Map SV Address to EEPROM Offset - Skip SV_ADDR_EEPROM_SIZE & SV_ADDR_SW_VERSION
			if( eeprom_read_byte((uint8_t*)lncvAddress) != lncvValue ) {
				eeprom_write_byte((uint8_t*)lncvAddress, lncvValue);
			}

			if(notifySVChanged) {
				notifySVChanged(lncvAddress + LNCV_SPECIAL_VALUE_OFFSET);
			}
			
			return LNCV_LACK_OK;
		}
	}
}

/**
 * Notifies the code on the reception of a request to end programming mode
 */
void notifyLNCVprogrammingStop( uint16_t ArtNr, uint16_t ModuleAddress ) {
	#ifdef DO_DEBUG
	Serial.print(F("notifyLNCVProgrammingStop\n"));
	#endif
	uint16_t myArtNr = LOAD_ARTNR;
	if (programmingModeActive && ArtNr == myArtNr && ModuleAddress == readModuleAddress()) {
		programmingModeActive = false;
	}
}