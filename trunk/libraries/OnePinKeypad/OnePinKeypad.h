#ifndef __ONEPINKEYPAD_H__
#define __ONEPINKEYPAD_H__

typedef enum {
  KEYPAD_RIGHT,
  KEYPAD_UP,
  KEYPAD_DOWN,
  KEYPAD_LEFT,
  KEYPAD_SELECT,
  KEYPAD_NONE,
} LcdKeyPress;

class OnePinKeypad
{
  public:
    OnePinKeypad();
    OnePinKeypad(uint8_t keyPin);

    LcdKeyPress getKeyPressed();
    void scanKeys(void);
    void setRepeatDelays(uint16_t initialDelay, uint16_t repDelay);
    
  private:
    uint8_t       keyPin;
    LcdKeyPress   lastKey;
    LcdKeyPress   lastKeyState;
    unsigned long lastKeyPressMillis;
    unsigned long nextScanKeysMillis;
    uint8_t       repeatCount;
    int           initialRepeatDelay;
    int           repeatDelay;
};

extern void    notifyKeyDown(LcdKeyPress key) __attribute__ ((weak));
extern void    notifyKeyRepeat(LcdKeyPress key, uint8_t Count) __attribute__ ((weak));
extern void    notifyKeyUp(LcdKeyPress key) __attribute__ ((weak));

#endif