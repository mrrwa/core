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

 Title :   System Timer
 Author:   Alex Shepherd <kiwi64ajs@sourceforge.net>
 Date:     16-Mar-2003
 Software:  AVR-GCC
 Target:    Arduino

 DESCRIPTION
       This module provides system timer actions

*****************************************************************************/

#ifndef __SYS_TIMER_DEFINED
#define __SYS_TIMER_DEFINED


#define TIMER_TICK_FREQUENCY        1000L // 1000kHz = 1ms tick size
#define TIMER_TICK_PERIOD_MS        (1000/TIMER_TICK_FREQUENCY) // Tick Period in ms
#define TIMER_RESOLUTION_MS         TIMER_TICK_PERIOD_MS
#define TIMER_FAST 1
#define TIMER_SLOW 0

typedef struct timer_action
{
  byte    Ticks ;
  void    *UserPointer ;
  byte    (*TickAction) ( void *UserPointer ) ;
  struct  timer_action *Next ; 
} TimerAction ;

void initTimer(void) ;

void resetTimerAction( TimerAction *addAction, byte Ticks ) ;  

void addTimerAction( TimerAction *addAction, byte Ticks, byte (*TickAction) ( void *UserPointer ), void *UserPointer, byte Fast ) ;  

void delayTimer( word delayTicks ) ;

void processTimerActions(void) ;

#endif
