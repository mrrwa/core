#include "HT1632Basic.h"

#define HTstartsys   0b100000000010 //start system oscillator
#define HTstopsys    0b100000000000 //stop sytem oscillator and LED duty    <default
#define HTsetclock   0b100000110000 //set clock to master with internal RC  <default
#define HTsetlayout  0b100001000000 //NMOS 32*8 // 0b100-0010-ab00-0  a:0-NMOS,1-PMOS; b:0-32*8,1-24*16   default:ab=10
#define HTledon      0b100000000110 //start LEDs
#define HTledoff     0b100000000100 //stop LEDs    <default
#define HTsetbright  0b100101000000 //set brightness b=0..15  add b<<1  //0b1001010xxxx0 xxxx:brightness 0..15=1/16..16/16 PWM
#define HTblinkon    0b100000010010 //Blinking on
#define HTblinkoff   0b100000010000 //Blinking off  <default
#define HTwrite      0b1010000000   // 101-aaaaaaa-dddd-dddd-dddd-dddd-dddd-... aaaaaaa:nibble adress 0..3F   (5F for 24*16)

void HT1632Basic::Setup(uint8_t strobePin, uint8_t clkPin, uint8_t dataPin)
{
	strobe = strobePin;
	pinMode(strobe, OUTPUT);
	digitalWrite(strobe, HIGH);
	
	clk = clkPin;
	pinMode(clk, OUTPUT);
	digitalWrite(clk, HIGH);

	data = dataPin;
	pinMode(data, OUTPUT);
	digitalWrite(data, HIGH);
	
	Command(HTstartsys);
	Command(HTledon);
	Command(HTsetclock);
	Command(HTsetlayout);
	Command(HTsetbright+(8<<1));
	Command(HTblinkoff);
}

void HT1632Basic::SendBuffer(LEDBuffer Buffer)
{
  digitalWrite(strobe, LOW);
  Send(HTwrite, 10);
  for (uint8_t mtx=0; mtx<4; mtx++)  //sending 8x8-matrices left to right, rows top to bottom, MSB left
    for (uint8_t row=0; row<8; row++) {  //while leds[] is organized in columns for ease of use.
      uint8_t q = 0;
      for (uint8_t col=0;col<8;col++)
		q = (q << 1) | ( ( Buffer[col + (mtx << 3)] >> row) & 1 ) ;
      Send(q,8);
    }
  digitalWrite(strobe,HIGH);
}

void HT1632Basic::Send(uint16_t dataVal, uint8_t bits)
{
  uint16_t bit = ((uint16_t)1)<<(bits-1);
  while(bit) 
  {
    digitalWrite(clk,LOW);
    if (dataVal & bit)
		digitalWrite(data,HIGH);
	else
		digitalWrite(data,LOW);
		
    digitalWrite(clk,HIGH);

    bit >>= 1;
  }
}

void HT1632Basic::Command(uint16_t data)
{
	digitalWrite(strobe,LOW);
	Send(data,12);
	digitalWrite(strobe,HIGH);
}

void HT1632Basic::Brightness(uint8_t level)
{
	Command(HTsetbright + ( (level & 15) <<1 ) );
}
