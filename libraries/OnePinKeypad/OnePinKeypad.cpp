#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "OnePinKeypad.h"

OnePinKeypad::OnePinKeypad()
{
  OnePinKeypad(0);
};

OnePinKeypad::OnePinKeypad(uint8_t newKeyPin)
{
  keyPin = newKeyPin;
  lastKey = KEYPAD_NONE;
  initialRepeatDelay = 500;
  repeatDelay = 200;
};

void OnePinKeypad::scanKeys(void)
{
  LCD_KEY_PRESS keyState = getKeyPressed();
	
  if( keyState != lastKey)
  {
    if(lastKey = KEYPAD_NONE)
    {
	  lastKeyPressMillis = millis();
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
	  lastKeyPressMillis = millis();
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
	unsigned long millisNow = millis();
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

LCD_KEY_PRESS OnePinKeypad::getKeyPressed()
{
  static int adc_key_val[] ={ 100, 300, 500, 700, 900 };
  int i, input;

  input = analogRead(keyPin);

  for (i = 0; i < (sizeof(adc_key_val)/sizeof(int)); i++)
  {
    if (input < adc_key_val[i])
      return LCD_KEY_PRESS(i);
  }
  
  return KEYPAD_NONE;
}