// Example showing how to use this library with the LCD & Keypad Shield 
#include <LiquidCrystal.h>
#include <OnePinKeypad.h>

// Define the pins for the LCD Display as usual
LiquidCrystal lcd = LiquidCrystal(8, 9, 4, 5, 6, 7);

// Define the Analog Input Pin for the Keypad interface (normally Analog pin 0 but my LEDuino has the Analog pins reversed)
OnePinKeypad keyPad = OnePinKeypad(5);

// Define the Key Names
char* keyNames[] = { "Right","Up","Down","Left","Select",""};

// function to update the Key Pressed part of the LCD Display
void UpdateStatus(LCD_KEY_PRESS key, uint8_t repeatCount)
{
  // Write the Key Name
  lcd.setCursor(0,1);
  uint8_t charsWritten = lcd.write(keyNames[key]);
  
  // Write the Repeat Count of > 0
  if(repeatCount)
  {
    lcd.write(' ');
    charsWritten += 1 + lcd.print(repeatCount);
  }
  
  // Write spaces over the remaining space
  while(charsWritten++ < 10)
    lcd.write(' ');
}

// OnePinKeypad call-back function on a Key Down
void notifyKeyDown(LCD_KEY_PRESS key)
{
  UpdateStatus(key, 0);
}

// OnePinKeypad call-back function on a Key Up
void notifyKeypadUp(LCD_KEY_PRESS key)
{
  UpdateStatus(key, 0);
}

// OnePinKeypad call-back function on a Key Repeat
void notifyKeyRepeat(LCD_KEY_PRESS key, uint8_t repeatCount)
{
  UpdateStatus(key, repeatCount);
}

void setup(void)
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("OnePinKeypad Ex");
  
  // Change the key repeat delays 
  keypad.setRepeatDelays(500,150);
}

void loop(void)
{
  // Need to call scanKeys() method to detect the Key Down/Repeat/Up
  keyPad.scanKeys();
}
