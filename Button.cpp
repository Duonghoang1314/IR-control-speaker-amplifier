#include "Button.h"
Button::Button(uint8_t pin, uint8_t puEnable, uint8_t invert, uint32_t dbTime)
{
  _pin = pin;
  _puEnable = puEnable;
  _invert = invert;
  _dbTime = dbTime;
  if (_puEnable != 0)
    pinMode(_pin, INPUT_PULLUP);    //enable pullup resistor
  else
    pinMode(_pin, INPUT);
  _state = digitalRead(_pin);
  if (_invert != 0) _state = !_state;
  _time = millis();
  _lastState = _state;
  _changed = 0;
  _lastChange = _time;
}
//123
void Button::setDebounce(uint32_t dbTime) {
  _dbTime = dbTime;
}
uint8_t Button::read(void)
{
  static uint32_t ms;
  static uint8_t pinVal;

  ms = millis();
  pinVal = digitalRead(_pin);
  if (_invert != 0) pinVal = !pinVal;
  if (ms - _lastChange < _dbTime) {
    _time = ms;
    _changed = 0;
    return _state;
  }
  else {
    _lastState = _state;
    _state = pinVal;
    _time = ms;
    if (_state != _lastState)   {
      _lastChange = ms;
      _changed = 1;
    }
    else {
      _changed = 0;
    }
    return _state;
  }
}
uint8_t Button::isPressed(void)
{
  return _state == 0 ? 0 : 1;
}
uint8_t Button::isReleased(void)
{
  return _state == 0 ? 1 : 0;
}
uint8_t Button::wasPressed(void)
{
  return _state && _changed;
}
uint8_t Button::wasReleased(void)
{
  return !_state && _changed;
}
uint8_t Button::pressedFor(uint32_t ms)
{
  return (_state == 1 && _time - _lastChange >= ms) ? 1 : 0;
}
uint8_t Button::releasedFor(uint32_t ms)
{
  return (_state == 0 && _time - _lastChange >= ms) ? 1 : 0;
}
uint32_t Button::lastChange(void)
{
  return _lastChange;
}
