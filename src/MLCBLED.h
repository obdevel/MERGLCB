
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

//
//
//

#pragma once

typedef enum {
  OFF,            ///< fixed OFF
  ON,             ///< fixed ON
  FLASH_50_1HZ,   ///< 50% duty cycle  1Hz
  FLASH_50_HALF_HZ,   ///< 50% duty cycle 0.5Hz
  SINGLE_FLICKER_OFF, ///< 250ms pulse off
  SINGLE_FLICKER_ON,  ///< 250ms pulse on
  LONG_FLICKER_OFF,   ///< 500ms pulse off
  LONG_FLICKER_ON     ///< 500ms pulse on
} LedState;

#ifndef DEBUG_SERIAL
#define DEBUG_SERIAL Serial
#endif

#include <Arduino.h>      // for definition of byte datatype

//
/// class to encapsulate a non-blocking LED
//

class MLCBLED {

public:
  MLCBLED();
  void virtual setPin(byte pin);
  bool getState();
  void on();
  void off();
  void toggle();
  void blink(byte rate=FLASH_50_1HZ);
  virtual void run();
  void pulse();
  void pulse_on();
  void pulse_off();
  void op(byte op);

protected:
  byte _pin;
  bool _state;
  bool _blink;
  unsigned int _blinkrate;
  bool _pulse;
  byte _pulsetype;
  unsigned int _pulselength;
  unsigned long _lastTime, _pulseStart;
  virtual void _write(byte pin, bool state);
};

