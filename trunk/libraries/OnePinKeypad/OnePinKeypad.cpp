#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "OnePinKeypad.h"

#define SAINSMART_1602_LCD_KEYPAD_SHIELD_CLONE

static const int RISE_TIME = 7; // rise time of analog input in milli seconds.
                                // To find required value: try pressing "RIGHT" 
                                // in example => ch should not change.

OnePinKeypad::OnePinKeypad()
{
  OnePinKeypad(0);
};

OnePinKeypad::OnePinKeypad(uint8_t newKeyPin)
{
  keyPin = newKeyPin;
  lastKey = KEYPAD_NONE;
  lastKeyState = KEYPAD_NONE;
  nextScanKeysMillis = millis() + RISE_TIME;
  initialRepeatDelay = 500;
  repeatDelay = 200;
};

void OnePinKeypad::scanKeys(void)
{
  unsigned long millisNow = millis(); // save for all calculations herein
  
  // avoid reading adc to fast
  if( millisNow < nextScanKeysMillis ) return;
  
  LcdKeyPress keyState = getKeyPressed();
  nextScanKeysMillis = millisNow + RISE_TIME;
  
  // be shure that we are not reading on a rising or falling edge
  bool same = (lastKeyState == keyState);
  lastKeyState = keyState;
  if(!same) return;

  if(keyState != lastKey)
  {
    if(lastKey = KEYPAD_NONE)
    {
      lastKeyPressMillis = millisNow;
      repeatCount = 0;

      if(notifyKeyDown)
        notifyKeyDown(keyState);
    }
    else if(keyState = KEYPAD_NONE)
    {
      if(notifyKeyUp)
        notifyKeyUp(lastKey);
    }
    else
    {
      lastKeyPressMillis = millisNow;
      repeatCount = 0;

      if(notifyKeyUp)
        notifyKeyUp(lastKey);

      if(notifyKeyDown)
        notifyKeyDown(keyState);
    }

    lastKey = keyState;
  }
  else if(notifyKeyRepeat && (keyState != KEYPAD_NONE))
  {
    unsigned long waitDelay = (repeatCount) ? repeatDelay : initialRepeatDelay;
    if((millisNow - lastKeyPressMillis) > waitDelay)
    {
      lastKeyPressMillis += waitDelay;
      notifyKeyRepeat(lastKey, ++repeatCount);
    }
  }
};

void OnePinKeypad::setRepeatDelays(uint16_t initialDelay, uint16_t repDelay)
{
  initialRepeatDelay = initialDelay;
  repeatDelay = repDelay;
}

LcdKeyPress OnePinKeypad::getKeyPressed()
{
  #ifdef SAINSMART_1602_LCD_KEYPAD_SHIELD_CLONE
    const static int A0 =    0;   // adc value when RIGHT key is pressed
    const static int A1 =  131;   // adc value when UP key is pressed
    const static int A2 =  307;   // adc value when DOWN key is pressed
    const static int A3 =  479;   // adc value when LEFT key is pressed
    const static int A4 =  720;   // adc value when SELECT key is pressed
    const static int A5 = 1023;   // adc value when no key is pressed
  #else
    const static int A0 =    0;   // adc value when RIGHT key is pressed
    const static int A1 =  200;   // adc value when UP key is pressed
    const static int A2 =  400;   // adc value when DOWN key is pressed
    const static int A3 =  600;   // adc value when LEFT key is pressed
    const static int A4 =  800;   // adc value when SELECT key is pressed
    const static int A5 = 1000;   // adc value when no key is pressed
  #endif
  const static int ADC_KEY_VAL[] = { (A0+A1)/2, (A1+A2)/2, (A2+A3)/2, 
                                     (A3+A4)/2, (A4+A5)/2             };
  int i, input;

  input = analogRead(keyPin);

  for (i = 0; i < (sizeof(ADC_KEY_VAL)/sizeof(ADC_KEY_VAL[0])); i++)
  {
    if (input < ADC_KEY_VAL[i])
      return LcdKeyPress(i);
  }
  
  return KEYPAD_NONE;
}