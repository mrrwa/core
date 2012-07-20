#include <HT1632Basic.h>

HT1632Basic Display;

LEDBuffer leds;

PROGMEM uint8_t bigdigits[10][6] =
{
  {126,129,129,129,129,126},  // 0
  {128,132,130,255,128,128},  // 1
  {130,193,161,145,137,134},  // 2
  { 66,129,137,137,137,118},  // 3
  {0x3f,0x20,0x20,0xfc,0x20,0x20}, // 4
  {0x4f,0x89,0x89,0x89,0x89,0x71}, // 5
  {0x7e,0x89,0x89,0x89,0x89,0x72}, // 6
  {0x03,0x01,0xc1,0x31,0x0d,0x03}, // 7
  {0x76,0x89,0x89,0x89,0x89,0x76}, // 8
  {0x46,0x89,0x89,0x89,0x89,0x7e}, // 9
};

volatile uint8_t sec=5;
uint8_t sec0=200, minute, hour, day, month; word year;
uint8_t changing, bright=3;

void renderclock(void)
{
  uint8_t col=0;
  for (uint8_t i=0;i<6;i++) 
    leds[col++]=pgm_read_byte(&bigdigits[hour/10][i]);
  
  leds[col++]=0;
  for (uint8_t i=0;i<6;i++)
    leds[col++]=pgm_read_byte(&bigdigits[hour%10][i]);

  leds[col++]=0;
  if (sec%2)
  {
    leds[col++]=0x66;
    leds[col++]=0x66;
  }
  else
  {
    leds[col++]=0;
    leds[col++]=0;
  }
  leds[col++]=0;
  for (uint8_t i=0;i<6;i++)
    leds[col++]=pgm_read_byte(&bigdigits[minute/10][i]);
    
  leds[col++]=0;
  for (uint8_t i=0;i<6;i++)
    leds[col++]=pgm_read_byte(&bigdigits[minute%10][i]);
  leds[col++]=0;
  
  leds[col++]=(sec>6) +((sec>13)<<1) +((sec>20)<<2) +((sec>26)<<3) +((sec>33)<<4) +((sec>40)<<5) +((sec>46)<<6) +((sec>53)<<7);
}

void clocksetup() {  // CLOCK, interrupt every second
  ASSR |= (1<<AS2);    //timer2 async from external quartz
  TCCR2=0b00000101;    //normal,off,/128; 32768Hz/256/128 = 1 Hz
  TIMSK |= (1<<TOIE2); //enable timer2-overflow-int
  sei();               //enable interrupts
}

// CLOCK interrupt
ISR(TIMER2_OVF_vect) {     //timer2-overflow-int
  sec++;
}

void incsec(uint8_t add) {
  sec+=add;
  while (sec>=60) { 
    sec-=60;  minute++;
    while (minute>=60) {
      minute -= 60;  hour++;
      while (hour >=24) {
        hour-=24;  day++;
      }//24hours
    }//60min
  }//60sec
}

void decsec(uint8_t sub) {
  while (sub>0) {
    if (sec>0) sec--; 
    else {
      sec=59; 
      if (minute>0) minute--; 
      else {
        minute=59; 
        if (hour>0) hour--;
        else {hour=23;day--;}
      }//hour
    }//minute
    sub--;
  }//sec
}

uint8_t clockhandler(void) {
  if (sec==sec0) 
    return 0;   //check if something changed
  sec0=sec;
  incsec(0);  //just carry over
  return 1;
}

void setup(void)
{
  pinMode(5, INPUT);
  digitalWrite(5,HIGH);
  
  pinMode(6, INPUT);
  digitalWrite(6,HIGH);

  pinMode(7, INPUT);
  digitalWrite(7,HIGH);
  
  for (uint8_t i=0; i<32; i++)
   leds[i] = 0b01010101 << (i % 2);

  Display.Setup(11,12,13);
  Display.SendBuffer(leds);
  
  clocksetup();
}

void loop(void)
{
  if( clockhandler())
  {
    renderclock();
    Display.SendBuffer(leds);
  }
  
  if (!digitalRead(7)) 
  {
    if (changing>250)
       incsec(20);
    else
    {
      changing++;
      incsec(1);
    }
  }
  else if (!digitalRead(6))
  {
    if (changing>250)
      decsec(20);
    else
    {
      changing++;
      decsec(1);
    }
  }
  else if (!digitalRead(5))
  {
    if (!changing)
    {
      changing=1;
      bright+=1;
      Display.Brightness(bright%16);
    }
  } //only once per press
  else
    changing=0;
}


