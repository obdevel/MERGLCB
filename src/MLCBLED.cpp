
/*

  Copyright (C) Duncan Greenwood 2017 (duncan_greenwood@hotmail.com)

  This work is licensed under the:
      Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
   To view a copy of this license, visit:
      http://creativecommons.org/licenses/by-nc-sa/4.0/
   or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

   License summary:
    You are free to:
      Share, copy and redistribute the material in any medium or format
      Adapt, remix, transform, and build upon the material

    The licensor cannot revoke these freedoms as long as you follow the license terms.

    Attribution : You must give appropriate credit, provide a link to the license,
                  and indicate if changes were made. You may do so in any reasonable manner,
                  but not in any way that suggests the licensor endorses you or your use.

    NonCommercial : You may not use the material for commercial purposes. **(see note below)

    ShareAlike : If you remix, transform, or build upon the material, you must distribute
                 your contributions under the same license as the original.

    No additional restrictions : You may not apply legal terms or technological measures that
                                 legally restrict others from doing anything the license permits.

   ** For commercial use, please contact the original copyright holder(s) to agree licensing terms

    This software is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE

*/

#include "MLCBLED.h"

//
/// class for individual LED with non-blocking control
//

MLCBLED::MLCBLED() {
  _state = LOW;
  _blink = false;
  _blinkrate = 0;
  _pulse = false;
  _pulsetype = ON;
  _pulselength = 0;
  _lastTime = 0UL;
}

//  set the pin for this LED

void MLCBLED::setPin(byte pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
}

// return the current state, on or off

bool MLCBLED::getState() {
  return _state;
}

// turn LED state on

void MLCBLED::on(void) {
  _state = HIGH;
  _blink = false;
}

// turn LED state off

void MLCBLED::off(void) {
  _state = LOW;
  _blink = false;
}

// toggle LED state from on to off or vv

void MLCBLED::toggle(void) {
  _state = !_state;
}

// blink LED

void MLCBLED::blink(byte rate) {
  _blink = true;
  _blinkrate = (rate == FLASH_50_1HZ) ? 500 : 250;
}

// pulse the LED

void MLCBLED::pulse() {
  pulse_on();
}

void MLCBLED::pulse_on() {
  _pulse = true;
  _state = HIGH;
  _pulseStart = millis();
  run();
}

void MLCBLED::pulse_off() {
  _pulse = true;
  _state = LOW;
  _pulseStart = millis();
  run();
}

// an LED operation
void MLCBLED::op(byte operation) {

  switch (operation) {
  case ON:
    on();
    break;
  case OFF:
    off();
    break;
  case FLASH_50_1HZ:            ///< 50% duty cycle  1Hz
    _blinkrate = 500;
    blink();
    break;
  case FLASH_50_HALF_HZ:        ///< 50% duty cycle 0.5Hz
    _blinkrate = 250;
    blink();
    break;
  case SINGLE_FLICKER_OFF:      ///< 250ms pulse off
    _pulselength = 250;
    pulse();
    break;
  case SINGLE_FLICKER_ON:       ///< 250ms pulse on
    _pulselength = 250;
    pulse();
    break;
  case LONG_FLICKER_OFF:        ///< 500ms pulse off
    _pulselength = 500;
    pulse();
    break;
  case LONG_FLICKER_ON:         ///< 500ms pulse on
    _pulselength = 500;
    pulse();
    break;
  }
}

// actually operate the LED dependent upon its current state
// must be called frequently from loop() if the LED is set to blink or pulse

void MLCBLED::run() {

  if (_blink) {

    // blinking
    if ((millis() - _lastTime) >= _blinkrate) {
      toggle();
      _lastTime = millis();
    }
  }

  // single pulse
  if (_pulse) {
    if (millis() - _pulseStart >= _pulselength) {
      _pulse = false;
      _state = _pulsetype == ON ? OFF : ON;
    }
  }

  _write(_pin, _state);
}

// write to the physical pin

void MLCBLED::_write(byte pin, bool state) {

  // DEBUG_SERIAL << F("> mcu pin = ") << pin << F(", state = ") << state << endl;
  digitalWrite(pin, state);
}
