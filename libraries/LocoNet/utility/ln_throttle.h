
#ifndef THROTTLE_INCLUDED
#define THROTTLE_INCLUDED

/****************************************************************************
    Copyright (C) 2002 Alex Shepherd

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************

 Title :   LocoNet Throttle module header file
 Author:   Alex Shepherd <kiwi64ajs@sourceforge.net>
 Date:     16-Mar-2003
 Software:  AVR-GCC
 Target:    Arduino

 DESCRIPTION
       This module provides all the throttle message decoding and provides
       functions to handle the common throttle operations

*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#include <LocoNet.h>
#include "ln_systimer.h"

#define MAX_THROTTLES 2

typedef enum
{
  TH_ST_FREE   = 0,
  TH_ST_ACQUIRE,
  TH_ST_SELECT,
  TH_ST_DISPATCH,
  TH_ST_SLOT_MOVE,
  TH_ST_SLOT_FREE,
  TH_ST_SLOT_RESUME,
  TH_ST_IN_USE
} TH_STATE ;

typedef enum
{
  TH_ER_OK = 0,
  TH_ER_SLOT_IN_USE,
  TH_ER_BUSY,
  TH_ER_NOT_SELECTED,
  TH_ER_NO_LOCO,
  TH_ER_NO_SLOTS
} TH_ERROR ;


typedef enum
{
	KEY_FUNCTION_0,
	KEY_FUNCTION_1,
	KEY_FUNCTION_2,
	KEY_FUNCTION_3,
	KEY_FUNCTION_4,
	KEY_FUNCTION_5,
	KEY_FUNCTION_6,
	KEY_FUNCTION_7,
	KEY_FUNCTION_8,
	KEY_FUNCTION_9,
	KEY_ADDR_RECALL,
	KEY_CLEAR,
	// These Key Actions below this point use the irRepeats
	// A check is made at the beginning of the irKeyDown function
	// and if the keyAction is less than KEY_ACQUIRE_RELEASE and irRepeats
	// is greater than 0 then the key press will be ignored so we
	// only execute these functions once
	KEY_ACQUIRE_RELEASE,
	KEY_FORWARD,
	KEY_REVERSE,
	KEY_STOP,
	KEY_MAX
} KEY_ACTIONS ;

typedef enum
{
  KEY_UP = 0,
  KEY_DOWN
} KEY_ACTION ;

typedef enum
{
  KEYPAD_MODE_NUMBER_ENTRY = 0,
  KEYPAD_MODE_DECODER_FUNCTION,
} KEYPAD_MODE ;


#define TH_OP_DEFERRED_SPEED 0x01

typedef struct throttle_data_t
{
  TH_STATE    State ;         // State of throttle
  uint16_t        TicksSinceLastAction ;
  uint16_t	      ThrottleId ;		// Id of throttle
  uint8_t        Slot ;          // Master Slot index
  uint16_t        Address ;       // Decoder Address
  uint8_t        Speed ;         // Loco Speed
  uint8_t        DeferredSpeed ; // Deferred Loco Speed setting
  uint8_t        Status1 ;       // Stat1
  uint8_t        DirFunc0to4 ;   // Direction
  uint8_t        Func5to8 ;       // Direction
  uint8_t        UserData ;
  uint8_t	      Options ;
  TimerAction ThrottleTimer ;
} THROTTLE_DATA_T;

void initThrottle( THROTTLE_DATA_T *ThrottleRec, uint8_t UserData, uint8_t Options, uint16_t ThrottleId ) ;

void processThrottleMessage( THROTTLE_DATA_T *ThrottleRec, lnMsg *LnPacket ) ;

uint16_t getThrottleAddress( THROTTLE_DATA_T *ThrottleRec ) ;
TH_ERROR setThrottleAddress( THROTTLE_DATA_T *ThrottleRec, uint16_t Address ) ;
TH_ERROR resumeThrottleAddress( THROTTLE_DATA_T *ThrottleRec, uint16_t Address, uint8_t LastSlot ) ;
TH_ERROR dispatchThrottleAddress( THROTTLE_DATA_T *ThrottleRec, uint16_t Address ) ;
TH_ERROR acquireThrottleAddress( THROTTLE_DATA_T *ThrottleRec ) ;
void releaseThrottleAddress( THROTTLE_DATA_T *ThrottleRec ) ;
TH_ERROR freeThrottleAddress( THROTTLE_DATA_T *ThrottleRec, uint16_t Address ) ;

uint8_t getThrottleSpeed( THROTTLE_DATA_T *ThrottleRec ) ;
TH_ERROR setThrottleSpeed( THROTTLE_DATA_T *ThrottleRec, uint8_t Speed ) ;

uint8_t getThrottleDirection( THROTTLE_DATA_T *ThrottleRec ) ;
TH_ERROR setThrottleDirection( THROTTLE_DATA_T *ThrottleRec, uint8_t Direction ) ;

uint8_t getThrottleFunction( THROTTLE_DATA_T *ThrottleRec, uint8_t Function ) ;
TH_ERROR setThrottleFunction( THROTTLE_DATA_T *ThrottleRec, uint8_t Function, uint8_t Value ) ;
TH_ERROR setThrottleDirFunc0to4Direct( THROTTLE_DATA_T *ThrottleRec, uint8_t Value ) ;
TH_ERROR setThrottleFunc5to8Direct( THROTTLE_DATA_T *ThrottleRec, uint8_t Value ) ;

TH_STATE getThrottleState( THROTTLE_DATA_T *ThrottleRec ) ;

void notifyThrottleAddress( uint8_t UserData, TH_STATE State, uint16_t Address, uint8_t Slot ) ;
void notifyThrottleSpeed( uint8_t UserData, TH_STATE State, uint8_t Speed ) ;
void notifyThrottleDirection( uint8_t UserData, TH_STATE State, uint8_t Direction ) ;
void notifyThrottleFunction( uint8_t UserData, uint8_t Function, uint8_t Value ) ;
void notifyThrottleSlotStatus( uint8_t UserData, uint8_t Status ) ;
void notifyThrottleError( uint8_t UserData, TH_ERROR Error ) ;
void notifyThrottleState( uint8_t UserData, TH_STATE PrevState, TH_STATE State ) ;

char *getStateStr( TH_STATE State ) ;
char *getErrorStr( TH_ERROR Error ) ;


#ifdef __cplusplus
}
#endif

#endif

