/****************************************************************************
 * Copyright (C) 2002 Alex Shepherd
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 *****************************************************************************
 * 
 * Title :   System Timer
 * Author:   Alex Shepherd <kiwi64ajs@sourceforge.net>
 * Date:     16-Mar-2003
 * Software:  AVR-GCC
 * Target:    Arduino
 * 
 * DESCRIPTION
 * This module provides system timer actions
 * 
 * CHANGE LOG
 * 2011-11-29 JMG  Modified the timer to use the second 8 bit timer, changed
 *                 the timing and prescaler to work (approximately) with the
 *                 16Mhz clock frequency of an arduino, currently it looses
 *                 around a second every 3 minutes.
 *****************************************************************************/

#include <avr/interrupt.h>	// switch global interrupt enable flag

#include "ln_systimer.h"


#define TIMER_PRESCALER_COUNT 250L    
#define TIMER_PRESCALER_CODE  3  

#define TICK_RELOAD           (256L - (F_CPU/(TIMER_TICK_FREQUENCY * TIMER_PRESCALER_COUNT)))

static TimerAction *SlowTimerActionList = 0 ; // 100 ms ticks
static TimerAction *FastTimerActionList = 0 ; // 1 ms ticks

uint8_t SlowTimerAccumulator = 0 ;
volatile uint8_t SlowTimerTicks = 0 ;
volatile uint8_t FastTimerTicks = 0 ;
volatile uint16_t DelayTimerTicks = 0 ;

ISR(TIMER2_OVF_vect)
{
  TCNT0 = (uint8_t) TICK_RELOAD ;

  FastTimerTicks++;

  if( ++SlowTimerAccumulator >= 100 )
  {
    SlowTimerAccumulator = 0 ;
    SlowTimerTicks++;
  }

  if( DelayTimerTicks )
  {
    DelayTimerTicks--;
  }
}

void initTimer()
{


  // Get the Current Timer1 Count and Add the offset for the Compare target
  TCNT2 = (uint8_t) TICK_RELOAD ;

  // Clear the Overflow interrupt status bit and enable the overflow interrupt
  TIFR2 |= _BV(TOV2);
  TIMSK2 |= _BV(TOIE2);

  // Set Timer Clock Source 

  TCCR2B = (TCCR2B & 0xF8) | TIMER_PRESCALER_CODE ;

  TCCR2B = _BV(CS01) | _BV(CS00);   // Clock/64, 0.001 seconds per tick
  OCR2A = 0xFA;         


}

void addTimerAction( TimerAction *pAction, uint8_t Ticks, uint8_t (*TickAction) ( void *UserPointer ), void *UserPointer, uint8_t Fast )
{
  uint8_t  StatusReg ;

  // This needs to be done with Interrupts off, save Status reg,
  // disable interrupts and then restore the previous Status reg
  // to enable interrupts again if they were off 
  StatusReg = SREG ;
  cli() ;

  if(Fast)
  {
    pAction->Next = FastTimerActionList ;
    FastTimerActionList = pAction ;
  }
  else
  {
    pAction->Next = SlowTimerActionList ;
    SlowTimerActionList = pAction ;
  }

  pAction->Ticks = Ticks ;
  pAction->TickAction = TickAction ;
  pAction->UserPointer = UserPointer ;

  // Enable Interrupts if they were on to start with
  SREG = StatusReg ;
}

void resetTimerAction( TimerAction *pAction, uint8_t Ticks )
{
  pAction->Ticks = Ticks ;
}

void delayTimer( uint16_t delayTicks )
{
  DelayTimerTicks = delayTicks ;

  while( DelayTimerTicks )
    ; // Sit and wait for the interrupt handler to decrement the ticks to 0
}

void processTimerActions(void)
{
  TimerAction *pAction ;
  uint8_t StatusReg ;
  uint8_t FastTicks ;
  uint8_t SlowTicks ;

  // This needs to be done with Interrupts off, save Status reg,
  // disable interrupts and then restore the previous Status reg
  // to enable interrupts again if they were off 
  StatusReg = SREG ;
  cli() ;

  FastTicks = FastTimerTicks ;   
  FastTimerTicks = 0 ;

  SlowTicks = SlowTimerTicks ;   
  SlowTimerTicks = 0 ;

  // Enable Interrupts if they were on to start with
  SREG = StatusReg ;

  if( FastTicks && FastTimerActionList )
  {
    while( FastTicks-- )
    {
      pAction = FastTimerActionList ;

      while( pAction )
      {
        if( pAction->Ticks )
        {
          pAction->Ticks--;
          // If we have gone from > 0 to 0 then set done and possibly reload
          if( !pAction->Ticks )
            pAction->Ticks = (*pAction->TickAction) ( pAction->UserPointer ) ;
        }

        pAction = pAction->Next ;
      }
    }
  }

  if( SlowTicks && SlowTimerActionList )
  {
    while( SlowTicks-- )
    {
      pAction = SlowTimerActionList ;

      while( pAction )
      {
        if( pAction->Ticks )
        {
          pAction->Ticks--;
          // If we have gone from > 0 to 0 then set done and possibly reload
          if( !pAction->Ticks )
            pAction->Ticks = (*pAction->TickAction) ( pAction->UserPointer ) ;
        }

        pAction = pAction->Next ;
      }
    }
  }
}                                                                                                                 

