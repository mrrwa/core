#ifndef OnePinKeypad_h
#define OnePinKeypad_h

typedef enum {
  KEYPAD_RIGHT,
  KEYPAD_UP,
  KEYPAD_DOWN,
  KEYPAD_LEFT,
  KEYPAD_SELECT,
  KEYPAD_NONE,
} LCD_KEY_PRESS;

class OnePinKeypad
{
  public:
    OnePinKeypad();
    OnePinKeypad(uint8_t keyPin);

    LCD_KEY_PRESS getKeyPressed();
	void scanKeys(void);
	void setRepeatDelays(uint16_t initialDelay, uint16_t repDelay);
    
  private:
    uint8_t		  keyPin;
	LCD_KEY_PRESS lastKey;
	unsigned long lastKeyPressMillis;
	uint8_t       repeatCount;
	int           initialRepeatDelay;
	int           repeatDelay;
};

extern void    notifyKeyDown(LCD_KEY_PRESS key) __attribute__ ((weak));
extern void    notifyKeyRepeat(LCD_KEY_PRESS key, uint8_t Count) __attribute__ ((weak));
extern void    notifyKeyUp(LCD_KEY_PRESS key) __attribute__ ((weak));

#endif