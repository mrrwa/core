#ifndef HT1632BASIC_H
#define HT1632BASIC_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

typedef uint8_t	LEDBuffer[32] ;

class HT1632Basic
{
  private:
	uint8_t strobe;
	uint8_t clk;
	uint8_t data;

	void Send(uint16_t data, uint8_t bits);
	void Command(uint16_t data);
  
  public:
	void Setup(uint8_t strobePin, uint8_t clkPin, uint8_t dataPin);
	void SendBuffer(LEDBuffer Buffer);
	void Brightness(uint8_t level);
	
};
#endif