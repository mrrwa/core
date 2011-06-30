/****************************************************************************
 * 	Copyright (C) 2009 Alex Shepherd
 * 
 * 	Portions Copyright (C) Digitrax Inc.
 * 
 * 	This library is free software; you can redistribute it and/or
 * 	modify it under the terms of the GNU Lesser General Public
 * 	License as published by the Free Software Foundation; either
 * 	version 2.1 of the License, or (at your option) any later version.
 * 
 * 	This library is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * 	Lesser General Public License for more details.
 * 
 * 	You should have received a copy of the GNU Lesser General Public
 * 	License along with this library; if not, write to the Free Software
 * 	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 *****************************************************************************
 * 
 * 	IMPORTANT:
 * 
 * 	Some of the message formats used in this code are Copyright Digitrax, Inc.
 * 	and are used with permission as part of the EmbeddedLocoNet project. That
 * 	permission does not extend to uses in other software products. If you wish
 * 	to use this code, algorithm or these message formats outside of
 * 	EmbeddedLocoNet, please contact Digitrax Inc, for specific permission.
 * 
 * 	Note: The sale any LocoNet device hardware (including bare PCB's) that
 * 	uses this or any other LocoNet software, requires testing and certification
 * 	by Digitrax Inc. and will be subject to a licensing agreement.
 * 
 * 	Please contact Digitrax Inc. for details.
 * 
 *****************************************************************************
 * 	DESCRIPTION
 * 	This module provides functions that manage the sending and receiving of LocoNet packets.
 * 	
 * 	As bytes are received from the LocoNet, they are stored in a circular
 * 	buffer and after a valid packet has been received it can be read out.
 * 	
 * 	When packets are sent successfully, they are also appended to the Receive
 * 	circular buffer so they can be handled like they had been received from
 * 	another device.
 * 
 * 	Statistics are maintained for both the send and receiving of packets.
 * 
 * 	Any invalid packets that are received are discarded and the stats are
 * 	updated approproately.
 * 
 *****************************************************************************/

#include "LocoNet.h"
#include "ln_sw_uart.h"
#include "ln_config.h"

LocoNetClass::LocoNetClass()
{
}

void LocoNetClass::init()
{
  initLnBuf(&LnBuffer) ;
  initLocoNetHardware(&LnBuffer);
}

lnMsg* LocoNetClass::receive()
{
  return recvLnMsg(&LnBuffer);
}

LN_STATUS LocoNetClass::send(lnMsg *pPacket)
{
  unsigned char ucTry;
  unsigned char ucPrioDelay = LN_BACKOFF_INITIAL;
  LN_STATUS enReturn;
  unsigned char ucWaitForEnterBackoff;

  for (ucTry = 0; ucTry < LN_TX_RETRIES_MAX; ucTry++)
  {

    // wait previous traffic and than prio delay and than try tx
    ucWaitForEnterBackoff = 1;  // don't want to abort do/while loop before
    do                          // we did not see the backoff state once
    {
      enReturn = sendLocoNetPacketTry(pPacket, ucPrioDelay);

      if (enReturn == LN_DONE)  // success?
        return LN_DONE;

      if (enReturn == LN_PRIO_BACKOFF)
        ucWaitForEnterBackoff = 0; // now entered backoff -> next state != LN_BACKOFF is worth incrementing the try counter
    }
    while ((enReturn == LN_CD_BACKOFF) ||                             // waiting CD backoff
    (enReturn == LN_PRIO_BACKOFF) ||                           // waiting master+prio backoff
    ((enReturn == LN_NETWORK_BUSY) && ucWaitForEnterBackoff)); // or within any traffic unfinished
    // failed -> next try going to higher prio = smaller prio delay
    if (ucPrioDelay > LN_BACKOFF_MIN)
      ucPrioDelay--;
  }
  LnBuffer.Stats.TxErrors++ ;
  return LN_RETRY_ERROR;
}

LN_STATUS LocoNetClass::send(lnMsg *pPacket, uint8_t ucPrioDelay)
{
  return sendLocoNetPacketTry(pPacket, ucPrioDelay);
}

LN_STATUS LocoNetClass::send( uint8_t OpCode, uint8_t Data1, uint8_t Data2 )
{
  lnMsg SendPacket ;

  SendPacket.data[ 0 ] = OpCode ;
  SendPacket.data[ 1 ] = Data1 ;
  SendPacket.data[ 2 ] = Data2 ;

  return send( &SendPacket ) ;
}

LN_STATUS LocoNetClass::send( uint8_t OpCode, uint8_t Data1, uint8_t Data2, uint8_t PrioDelay )
{
  lnMsg SendPacket ;

  SendPacket.data[ 0 ] = OpCode ;
  SendPacket.data[ 1 ] = Data1 ;
  SendPacket.data[ 2 ] = Data2 ;

  return sendLocoNetPacketTry( &SendPacket, PrioDelay ) ;
}

uint8_t LocoNetClass::processSwitchSensorMessage( lnMsg *LnPacket )
{
  uint16_t Address ;
  uint8_t  Direction ;
  uint8_t  Output ;
  uint8_t  ConsumedFlag = 1 ;

  Address = (LnPacket->srq.sw1 | ( ( LnPacket->srq.sw2 & 0x0F ) << 7 )) ;
  if( LnPacket->sr.command != OPC_INPUT_REP )
    Address++;

  switch( LnPacket->sr.command )
  {
  case OPC_INPUT_REP:
    Address <<= 1 ;
    Address += ( LnPacket->ir.in2 & OPC_INPUT_REP_SW ) ? 2 : 1 ;

    if(notifySensor)
      notifySensor( Address, LnPacket->ir.in2 & OPC_INPUT_REP_HI ) ;
    break ;

  case OPC_SW_REQ:
    if(notifySwitchRequest)
      notifySwitchRequest( Address, LnPacket->srq.sw2 & OPC_SW_REQ_OUT, LnPacket->srq.sw2 & OPC_SW_REQ_DIR ) ;
    break ;

  case OPC_SW_REP:
    if(notifySwitchReport)
      notifySwitchReport( Address, LnPacket->srp.sn2 & OPC_SW_REP_HI, LnPacket->srp.sn2 & OPC_SW_REP_SW ) ;
    break ;

  case OPC_SW_STATE:
    Direction = LnPacket->srq.sw2 & OPC_SW_REQ_DIR ;
    Output = LnPacket->srq.sw2 & OPC_SW_REQ_OUT ;

    if(notifySwitchState)
      notifySwitchState( Address, Output, Direction ) ;
    break;

  case OPC_SW_ACK:
    break ;

  case OPC_LONG_ACK:
    if( LnPacket->lack.opcode == (OPC_SW_STATE & 0x7F ) )
    {
      Direction = LnPacket->lack.ack1 & 0x01 ;
    }
    else
      ConsumedFlag = 0 ;
    break;

  default:
    ConsumedFlag = 0 ;
  }

  return ConsumedFlag ;
}

void LocoNetClass::requestSwitch( uint16_t Address, uint8_t Output, uint8_t Direction )
{
  uint8_t AddrH = (--Address >> 7) & 0x0F ;
  uint8_t AddrL = Address & 0x7F ;

  if( Output )
    AddrH |= OPC_SW_REQ_OUT ;

  if( Direction )
    AddrH |= OPC_SW_REQ_DIR ;

  send( OPC_SW_REQ, AddrL, AddrH ) ;
}

void LocoNetClass::reportSwitch( uint16_t Address )
{
  Address -= 1;
  send( OPC_SW_STATE, (Address & 0x7F), ((Address >> 7) & 0x0F) ) ;
}

LocoNetClass LocoNet = LocoNetClass();
