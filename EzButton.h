#ifndef EzButton_h
#define EzButton_h
#include "Arduino.h"

class button
{
  private:
    uint8_t btn;
    uint16_t state;
  public:
    void begin(uint8_t button) {
      btn = button;
      state = 0;
      pinMode(btn, INPUT_PULLUP);
    }
    bool debounce() {
      state = (state << 1) | digitalRead(btn) | 0xfe00;
      return (state == 0xff00);
    }
};
#endif
/*
  #include "button.h"

  Button btn1;
  Button btn2;

  void setup() {
  btn1.begin(2);
  btn2.begin(3);
  pinMode(LED_BUILTIN, OUTPUT);
  }

  void loop() {
  if (btn1.debounce()) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (btn2.debounce()) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  }
*/