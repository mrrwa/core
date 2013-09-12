// Example showing how to use this library with the LCD & Keypad Shield 
#include <stdint.h>         // typedef uint8_t
#include <LiquidCrystal.h>  // class LiquidCrystal
#include <OnePinKeypad.h>   // class OnePinKeypad

// Define the Analog Input Pin for the Keypad interface (normally Analog 
// pin 0 but my LEDuino has the Analog pins reversed)
const int KEYPAD_APIN   =  0; // analog pin used for Keypad reading

// Define the pins for the LCD Display as usual
const int LCD_RS        =  8; // digital pin used for "LCD RS"
const int LCD_ENABLE    =  9; // digital pin used for "LCD Enable"
                              // "LCD R/W" pin to ground
const int LCD_D4        =  4; // digital pin used for "LCD D4"
const int LCD_D5        =  5; // digital pin used for "LCD D5"
const int LCD_D6        =  6; // digital pin used for "LCD D6"
const int LCD_D7        =  7; // digital pin used for "LCD D7"

LiquidCrystal lcd( LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7 );
OnePinKeypad keyPad( KEYPAD_APIN );

// Define the Key Names
const char* keyNames[] = { "Right", "Up", "Down", "Left", "Select", "" };

// function to update the Key Pressed part of the LCD Display
void UpdateStatus(LcdKeyPress key, uint8_t repeatCount)
{
  lcd.setCursor(0,1);
  // Write the Key Name
  uint8_t charsWritten = lcd.write(keyNames[key]);
  
  // Write the Repeat Count of > 0
  if(repeatCount)
  {
    charsWritten += lcd.write(' ');
    charsWritten += lcd.print(repeatCount);
  }
  
  // Write spaces over the remaining space
  while(charsWritten++ < 10)
    lcd.write(' ');
}

uint8_t ch = '@';

void updateChar() 
{
    lcd.setCursor(11,1);
    // write the char itself
    lcd.write(ch); 
    lcd.print(' ');
    // write decimal value
    int charsWritten = lcd.print( ch );
    // fill with spaces
    while(charsWritten++ <3) lcd.print(' ');
}

// OnePinKeypad call-back function on a Key Down
void notifyKeyDown(LcdKeyPress key)
{
  if( key == KEYPAD_UP ) ch++;
  if( key == KEYPAD_DOWN ) ch--;
  if( key == KEYPAD_SELECT ) ch += 10;
  updateChar();
  UpdateStatus(key, 0);
}

// OnePinKeypad call-back function on a Key Up
void notifyKeypadUp(LcdKeyPress key)
{
  UpdateStatus(key, 0);
}

// OnePinKeypad call-back function on a Key Repeat
void notifyKeyRepeat(LcdKeyPress key, uint8_t repeatCount)
{
  if( key == KEYPAD_UP ) ch++;
  if( key == KEYPAD_DOWN ) ch--;
  updateChar();
  UpdateStatus(key, repeatCount);
}

void setup(void)
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("OnePinKeypad Ex");
  updateChar();
  
  // Change the key repeat delays 
  keyPad.setRepeatDelays(500,150);
}

void loop(void)
{
  // Need to call scanKeys() method to detect the Key Down/Repeat/Up
  keyPad.scanKeys();
  /*
  // to get the A0 .. A5 values for OnePinKeypad::getKeyPressed() 
  // of your shield enable this code lines and take a note of the 
  // values while pressing all keys 
  int i = analogRead(0);
  lcd.setCursor(8,1); lcd.print( i ); lcd.print("     "); delay(500);
  */
}
