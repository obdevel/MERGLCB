
/*
  Copyright (C) Duncan Greenwood 2023 (duncan_greenwood@hotmail.com)

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

// 3rd party libraries
#include <Streaming.h>

// MLCB library
#include <MLCB.h>

// forward function declarations
void makeHeader_impl(CANFrame *msg, byte id, byte priority = 0x0b);

//
/// construct a MLCB object with a MLCBConfig object that the user provides.
/// note that this MLCBConfig object must have a lifetime longer than the MLCB object.
//

MLCBbase::MLCBbase(MLCBConfig *the_config) {
  module_config = the_config;
}

//
/// register the user handler for learned events
//

void MLCBbase::setEventHandler(void (*fptr)(byte index, CANFrame *msg)) {
  eventhandler = fptr;
}

// overloaded form which receives the opcode on/off state and the first event variable

void MLCBbase::setEventHandler(void (*fptr)(byte index, CANFrame *msg, bool ison, byte evval)) {
  eventhandlerex = fptr;
}

//
/// register the user handler for CAN frames
/// default args in .h declaration for opcodes array (NULL) and size (0)
//

void MLCBbase::setFrameHandler(void (*fptr)(CANFrame *msg), byte opcodes[], byte num_opcodes) {
  framehandler = fptr;
  _opcodes = opcodes;
  _num_opcodes = num_opcodes;
}

//
/// assign the module parameter set
//

void MLCBbase::setParams(unsigned char *mparams) {
  _mparams = mparams;
}

//
/// assign the module name
//

void MLCBbase::setName(unsigned char *mname) {
  _mname = mname;
}

//
/// set module to SLiM mode
//

void MLCBbase::setSLiM(void) {

  mode_changing = false;
  module_config->setNodeNum(0);
  module_config->setFLiM(false);
  module_config->setCANID(0);
  indicateMode(module_config->FLiM);
}

//
/// extract CANID from CAN frame header
//

inline byte MLCBbase::getCANID(unsigned long header) {
  return header & 0x7f;
}

//
/// send a WRACK (write acknowledge) message -- only for backward compatibility with CBUS
//

bool MLCBbase::sendWRACK(void) {

  // send a write acknowledgement response

  _msg.len = 3;
  _msg.data[0] = OPC_WRACK;
  _msg.data[1] = highByte(module_config->nodeNum);
  _msg.data[2] = lowByte(module_config->nodeNum);
  return sendMessage(&_msg);
}

//
/// send a CMDERR (command error) message -- only for backward compatibility with CBUS
//

bool MLCBbase::sendCMDERR(byte cerrno) {

  // send a command error response
  _msg.len = 4;
  _msg.data[0] = OPC_GRSP;
  _msg.data[1] = highByte(module_config->nodeNum);
  _msg.data[2] = lowByte(module_config->nodeNum);
  _msg.data[3] = cerrno;
  return sendMessage(&_msg);
}

//
/// send a GRSP response message
//

bool MLCBbase::sendGRSP(byte cerrno, byte opcode, byte data1, byte data2) {

  _msg.len = 7;
  _msg.data[0] = OPC_GRSP;
  _msg.data[1] = highByte(module_config->nodeNum);
  _msg.data[2] = lowByte(module_config->nodeNum);
  _msg.data[3] = cerrno;
  _msg.data[4] = opcode;
  _msg.data[5] = data1;
  _msg.data[6] = data2;
  return sendMessage(&_msg);
}

//
/// is this an Extended CAN frame ?
//

bool MLCBbase::isExt(CANFrame *amsg) {

  return (amsg->ext);
}

//
/// is this a Remote frame ?
//

bool MLCBbase::isRTR(CANFrame *amsg) {

  return (amsg->rtr);
}

//
/// if in FLiM mode, initiate a CAN ID enumeration cycle
//

void MLCBbase::start_enumeration(void) {

  // initiate CAN bus enumeration cycle, either due to ENUM opcode, ID clash, or user button press
  enumeration_required = false;
  enumeration_active = true;        // we are enumerating
  enumeration_start = millis();     // the cycle start time
  memset(enumeration_responses, 0, sizeof(enumeration_responses));
  _msg.len = 0;
  sendMessage(&_msg, true, false);

  return;
}

//
/// initiate the transition from SLiM to FLiM mode
//

void MLCBbase::initFLiM(void) {

  indicateMode(MODE_CHANGING);
  mode_changing = true;
  timeout_timer = millis();

  // send RQNN message with current NN, which may be zero if a virgin/SLiM node
  _msg.len = 3;
  _msg.data[0] = OPC_RQNN;
  _msg.data[1] = highByte(module_config->nodeNum);
  _msg.data[2] = lowByte(module_config->nodeNum);
  sendMessage(&_msg);

  return;
}

//
/// revert from FLiM to SLiM mode
//

void MLCBbase::revertSLiM(void) {

  // DEBUG_SERIAL << F("> reverting to SLiM mode") << endl;

  // send NNREL message
  _msg.len = 3;
  _msg.data[0] = OPC_NNREL;
  _msg.data[1] = highByte(module_config->nodeNum);
  _msg.data[2] = lowByte(module_config->nodeNum);
  sendMessage(&_msg);
  setSLiM();

  return;
}

//
/// change or re-confirm node number
//

void MLCBbase::renegotiate(void) {

  initFLiM();
}

//
/// assign the two MLCB LED objects
//

void MLCBbase::setLEDs(MLCBLED green, MLCBLED yellow) {

  UI = true;
  _ledGrn = green;
  _ledYlw = yellow;
}

//
/// assign the MLCB pushbutton switch object
//

void MLCBbase::setSwitch(MLCBSwitch sw) {

  UI = true;
  _sw = sw;
}

//
/// set the MLCB LEDs to indicate the current mode
//

void MLCBbase::indicateMode(byte mode) {

  if (UI) {
    switch (mode) {

    case MODE_FLIM:
      _ledYlw.on();
      _ledGrn.off();
      break;

    case MODE_SLIM:
      _ledYlw.off();
      _ledGrn.on();
      break;

    case MODE_CHANGING:
      _ledYlw.blink();
      _ledGrn.off();
      break;

    default:
      break;
    }
  }
}

//
/// main MLCB message processing procedure
//

void MLCBbase::process(byte num_messages) {

  byte remoteCANID = 0, nvindex = 0, evindex = 0, evval = 0;
  unsigned int nn = 0, en = 0, j = 0, opc;

  // start bus enumeration if required
  if (enumeration_required) {
    start_enumeration();
  }

  // process switch operations if the module is configured with one

  if (UI) {
    // allow LEDs to update
    _ledGrn.run();
    _ledYlw.run();

    // allow the MLCB switch some processing time
    _sw.run();

    // use LEDs to indicate that the user can release the switch
    if (_sw.isPressed() && _sw.getCurrentStateDuration() > SW_TR_HOLD) {
      indicateMode(MODE_CHANGING);
    }

    //
    /// handle switch state changes
    //

    if (_sw.stateChanged()) {
      if (!_sw.isPressed()) {

        unsigned long press_time = _sw.getLastStateDuration();

        // long hold > 8 secs
        if (press_time > SW_TR_HOLD) {
          if (!module_config->FLiM) {
            initFLiM();
          } else {
            revertSLiM();
          }
        }

        // short 1-2 secs
        if (press_time >= 1000 && press_time < 2000) {
          renegotiate();
        }

        // very short < 0.5 sec
        if (press_time < 500 && module_config->FLiM) {
          start_enumeration();
        }

      }
    }
  }

  // heartbeat

  if (isMLCB && hbactive && module_config->FLiM && millis() - hbtimer > HBTIMER_INTERVAL) {
    hbtimer = millis();
    _msg.len = 6;
    _msg.data[0] = OPC_HEARTB;
    _msg.data[1] = highByte(module_config->nodeNum);
    _msg.data[2] = lowByte(module_config->nodeNum);
    _msg.data[3] = hbcount++;
    _msg.data[4] = 0;
    _msg.data[5] = 0;
    sendMessage(&_msg);
  }

  // get received CAN frames from buffer
  // process by default 3 messages per run so the user's application code doesn't appear unresponsive under load

  byte mcount = 0;

  while (available() && mcount < num_messages) {

    ++mcount;

    // at least one CAN frame is available in the reception buffer
    // retrieve the next one

    // memset(&_msg, 0, sizeof(CANFrame));
    _msg = getNextMessage();

    // extract OPC, NN, EN
    opc = _msg.data[0];
    nn = (_msg.data[1] << 8) + _msg.data[2];
    en = (_msg.data[3] << 8) + _msg.data[4];

    //
    /// extract the CANID of the sending module
    //

    remoteCANID = getCANID(_msg.id);

    //
    /// if registered, call the user handler with this new frame
    //

    if (framehandler != NULL) {

      // check if incoming opcode is in the user list, if list length > 0
      if (_num_opcodes > 0) {
        for (byte i = 0; i < _num_opcodes; i++) {
          if (opc == _opcodes[i]) {
            (void)(*framehandler)(&_msg);
            break;
          }
        }
      } else {
        (void)(*framehandler)(&_msg);
      }
    }

    //
    /// pulse the green LED
    //

    if (UI) {
      _ledGrn.pulse();
    }

    // is this a CANID enumeration request from another node (RTR set) ?
    if (_msg.rtr) {
      // send an empty message to show our CANID
      _msg.len = 0;
      sendMessage(&_msg);
      continue;
    }

    //
    /// set flag if we find a CANID conflict with the frame's producer
    /// doesn't apply to RTR or zero-length frames, so as not to trigger an enumeration loop
    //

    if (remoteCANID == module_config->CANID && _msg.len > 0) {
      enumeration_required = true;
    }

    // is this an extended frame ? we currently ignore these as bootloader, etc data may confuse us !
    if (_msg.ext) {
      continue;
    }

    // are we enumerating CANIDs ?
    if (enumeration_active && _msg.len == 0) {

      // store this response in the responses array
      if (remoteCANID > 0) {
        bitWrite(enumeration_responses[(remoteCANID / 16)], remoteCANID % 8, 1);
      }

      continue;
    }

    //
    /// process the message opcode
    /// if we got this far, it's a standard CAN frame (not extended, not RTR) with a data payload length > 0
    //

    if (_msg.len > 0) {

      byte index;

      switch (opc) {

      case OPC_ACON:
      case OPC_ACON1:
      case OPC_ACON2:
      case OPC_ACON3:

      case OPC_ACOF:
      case OPC_ACOF1:
      case OPC_ACOF2:
      case OPC_ACOF3:

      case OPC_ARON:
      case OPC_AROF:

        // lookup this accessory event in the event table and call the user's registered callback function
        if (eventhandler || eventhandlerex) {
          processAccessoryEvent(nn, en, (opc % 2 == 0));
        }

        break;

      case OPC_ASON:
      case OPC_ASON1:
      case OPC_ASON2:
      case OPC_ASON3:

      case OPC_ASOF:
      case OPC_ASOF1:
      case OPC_ASOF2:
      case OPC_ASOF3:

        // lookup this accessory event in the event table and call the user's registered callback function
        if (eventhandler || eventhandlerex) {
          processAccessoryEvent(0, en, (opc % 2 == 0));
        }

        break;

      case OPC_RQNP:
        // RQNP message - request for node paramters -- does not contain a NN or EN, so only respond if we
        // are in transition to FLiM

        // only respond if we are in transition to FLiM mode
        if (mode_changing == true) {

          // respond with PARAMS message
          _msg.len = 8;
          _msg.data[0] = OPC_PARAMS;    // opcode
          _msg.data[1] = _mparams[1];     // manf code -- MERG
          _msg.data[2] = _mparams[2];     // minor code ver
          _msg.data[3] = _mparams[3];     // module ident
          _msg.data[4] = _mparams[4];     // number of events
          _msg.data[5] = _mparams[5];     // events vars per event
          _msg.data[6] = _mparams[6];     // number of NVs
          _msg.data[7] = _mparams[7];     // major code ver
          sendMessage(&_msg);
        }

        break;

      case OPC_RQNPN:
        // RQNPN message -- request parameter by index number
        // index 0 = number of params available;
        // respond with PARAN

        if (nn == module_config->nodeNum) {

          byte paran = _msg.data[3];

          if (paran <= _mparams[0]) {
            paran = _msg.data[3];

            _msg.len = 5;
            _msg.data[0] = OPC_PARAN;
            _msg.data[3] = paran;
            _msg.data[4] = _mparams[paran];
            sendMessage(&_msg);

          } else {
            sendCMDERR(9);
          }
        }

        break;

      case OPC_SNN:
        // received SNN - set node number

        if (mode_changing) {
          // save the NN
          module_config->setNodeNum(nn);
          ++_numNNchanges;

          // respond with NNACK
          _msg.len = 3;
          _msg.data[0] = OPC_NNACK;
          sendMessage(&_msg);

          // we are now in FLiM mode - update the configuration
          mode_changing = false;
          module_config->setFLiM(true);
          indicateMode(module_config->FLiM);

          // enumerate the CAN bus to allocate a free CAN ID
          enumeration_required = true;
        } else {
          // DEBUG_SERIAL << F("> received SNN but not in transition") << endl;
        }

        break;

      case OPC_CANID:
        // CAN -- set CANID

        if (nn == module_config->nodeNum) {
          // DEBUG_SERIAL << F("> setting my CANID to ") << _msg.data[3] << endl;
          if (_msg.data[3] < 1 || _msg.data[3] > 99) {
            sendCMDERR(7);
          } else {
            module_config->setCANID(_msg.data[3]);
          }
        }

        break;

      case OPC_ENUM:
        // received ENUM -- start CAN bus self-enumeration

        if (nn == module_config->nodeNum && remoteCANID != module_config->CANID && !enumeration_active) {
          // DEBUG_SERIAL << F("> initiating enumeration") << endl;
          enumeration_required = true;
        }

        break;

      case OPC_NVRD:
        // received NVRD -- read NV by index
        if (nn == module_config->nodeNum) {

          if (nvindex > module_config->EE_NUM_NVS) {
            sendCMDERR(10);
          } else {
            _msg.len = 5;
            _msg.data[0] = OPC_NVANS;
            _msg.data[4] = module_config->readNV(_msg.data[3]);
            sendMessage(&_msg);
          }
        }

        break;

      case OPC_NVSET:
        // received NVSET -- set NV by index

        if (nn == module_config->nodeNum) {
          if (_msg.data[3] > module_config->EE_NUM_NVS) {
            sendCMDERR(10);
          } else {
            module_config->writeNV( _msg.data[3], _msg.data[4]);
            sendWRACK();
          }
        }

        break;

      case OPC_NNLRN:
        // received NNLRN -- place into learn mode

        if (nn == module_config->nodeNum) {
          bLearn = true;
          bitSet(_mparams[8], 5);
        }

        break;

      case OPC_EVULN:
        // received EVULN -- unlearn an event, by event number

        // we must be in learn mode
        if (bLearn == true) {
          // search for this NN and EN pair
          index = module_config->findExistingEvent(nn, en);

          if (index < module_config->EE_MAX_EVENTS) {
            module_config->cleareventEEPROM(j);
            // update hash table
            module_config->updateEvHashEntry(j);
            // respond with WRACK
            sendWRACK();

          } else {
            sendCMDERR(10);
          }

        } // if in learn mode

        break;

      case OPC_NNULN:
        // received NNULN -- exit from learn mode

        if (nn == module_config->nodeNum) {
          bLearn = false;
          bitClear(_mparams[8], 5);
        }

        break;

      case OPC_RQEVN:
        // received RQEVN -- request for number of stored events

        if (nn == module_config->nodeNum) {
          _msg.len = 4;
          _msg.data[0] = OPC_NUMEV;
          _msg.data[3] = module_config->numEvents();
          sendMessage(&_msg);
        }

        break;

      case OPC_NERD:
        // request for all stored events

        if (nn == module_config->nodeNum) {
          _msg.len = 8;
          _msg.data[0] = OPC_ENRSP;                       // response opcode
          _msg.data[1] = highByte(module_config->nodeNum);        // my NN hi
          _msg.data[2] = lowByte(module_config->nodeNum);         // my NN lo

          for (byte i = 0; i < module_config->EE_MAX_EVENTS; i++) {

            if (module_config->getEvTableEntry(i) != 0) {
              module_config->readEvent(i, &_msg.data[3]);
              _msg.data[7] = i;                           // event table index
              sendMessage(&_msg);
              delay(10);

            } // valid stored ev
          } // loop each ev
        } // for me

        break;

      case OPC_REVAL:
        // received REVAL -- request read of an event variable by event index and ev num
        // respond with NEVAL

        if (nn == module_config->nodeNum) {

          if (module_config->getEvTableEntry(_msg.data[3]) != 0) {
            _msg.len = 6;
            _msg.data[0] = OPC_NEVAL;
            _msg.data[5] = module_config->getEventEVval(_msg.data[3], _msg.data[4]);
            sendMessage(&_msg);
          } else {
            sendCMDERR(6);
          }

        }

        break;

      case OPC_NNCLR:
        // NNCLR -- clear all stored events

        if (bLearn == true && nn == module_config->nodeNum) {
          for (byte e = 0; e < module_config->EE_MAX_EVENTS; e++) {
            module_config->cleareventEEPROM(e);
          }

          module_config->clearEvHashTable();
          sendWRACK();
        }

        break;

      case OPC_NNEVN:
        // request for number of free event slots

        if (module_config->nodeNum == nn) {
          byte free_slots = 0;

          // count free slots using the event hash table
          for (byte i = 0; i < module_config->EE_MAX_EVENTS; i++) {
            if (module_config->getEvTableEntry(i) == 0) {
              ++free_slots;
            }
          }

          _msg.len = 4;
          _msg.data[0] = OPC_EVNLF;
          _msg.data[3] = free_slots;
          sendMessage(&_msg);
        }

        break;

      case OPC_QNN:
        // this is probably a config recreate -- respond with PNN if we have a node number
        if (module_config->nodeNum > 0) {
          _msg.len = 6;
          _msg.data[0] = OPC_PNN;
          _msg.data[1] = highByte(module_config->nodeNum);
          _msg.data[2] = lowByte(module_config->nodeNum);
          _msg.data[3] = _mparams[1];
          _msg.data[4] = _mparams[3];
          _msg.data[5] = _mparams[8];
          sendMessage(&_msg);
        }

        break;

      case OPC_RQMN:
        // request for node module name, excluding "CAN" prefix
        // sent during module transition, so no node number check
        // only respond if in transition to FLiM

        // respond with NAME
        if (mode_changing) {
          _msg.len = 8;
          _msg.data[0] = OPC_NAME;
          memcpy(_msg.data + 1, _mname, 7);
          sendMessage(&_msg);
        }

        break;

      case OPC_EVLRN:
        // received EVLRN -- learn an event
        evindex = _msg.data[5];
        evval = _msg.data[6];

        // we must be in learn mode
        if (bLearn == true) {
          index = module_config->findExistingEvent(nn, en);

          if (index >= module_config->EE_MAX_EVENTS) {
            index = module_config->findEventSpace();
          }

          if (index < module_config->EE_MAX_EVENTS) {

            // write the event to EEPROM at this location -- EVs are indexed from 1 but storage offsets start at zero !!
            // don't repeat this for subsequent EVs
            if (evindex < 2) {
              module_config->writeEvent(index, &_msg.data[1]);
            }

            module_config->writeEventEV(index, evindex, evval);
            // recreate event hash table entry
            module_config->updateEvHashEntry(index);
            // respond with WRACK
            sendWRACK();

          } else {
            // respond with CMDERR
            sendCMDERR(10);
          }

        } else { // bLearn == true
        }

        break;

      case OPC_AREQ:
        // AREQ message - request for node state, only producer nodes

        if (module_config->nodeNum == nn) {
          (void)(*eventhandler)(0, &_msg);
        }

        break;

      case OPC_BOOT:
        // boot mode
        break;

      case OPC_RSTAT:
        // command station status -- not applicable to accessory modules
        break;

      case OPC_DTXC:
        // MLCB multipart message
        if (MultipartMessageHandler != NULL) {
          MultipartMessageHandler->processReceivedMessageFragment(&_msg);
        }
        break;

      /// new opcodes for MLCB MNS

      case OPC_MODE:
        if (module_config->nodeNum == nn) {
          Serial << F("> got OPC_MODE") << endl;
        }

        break;

      case OPC_RDGN:
        if (module_config->nodeNum == nn) {
          Serial << F("> got OPC_RDGN") << endl;
          byte service_num = _msg.data[3];
          byte data1 = 0;
          byte data2 = 0;

          switch (_msg.data[4]) {
          case 2:
            data1 = millis() >> 24;
            data2 = millis() >> 16;
            break;
          case 3:
            data1 = millis() >> 8;
            data2 = millis() & 0xff;
            break;
          case 5:
            data1 = _numNNchanges >> 8;
            data2 = _numNNchanges & 0xff;
            break;
          case 7:
            data1 = _numMsgsActioned >> 8;
            data2 = _numMsgsActioned & 0xff;
            break;
          }

          _msg.len = 5;
          _msg.data[0] = OPC_DGN;
          _msg.data[1] = highByte(module_config->nodeNum);
          _msg.data[2] = lowByte(module_config->nodeNum);
          _msg.data[3] = data1;
          _msg.data[4] = data2;
          sendMessage(&_msg);
        }

        break;

      case OPC_RQSD:
        if (module_config->nodeNum == nn) {
          Serial << F("> got OPC_RDGN") << endl;
          // byte service_num = _msg.data[3];
          // byte diag_code = _msg.data[4];
        }

        break;

      case OPC_REQEV:
        if (module_config->nodeNum == nn) {
          Serial << F("> got OPC_REQEV") << endl;
        }

        break;

      default:
        // unknown or unhandled OPC
        break;
      }
    } else {
    }
  }  // while messages available

  // check CAN bus enumeration timer
  check_enumeration();

  //
  /// check 30 sec timeout for SLiM/FLiM negotiation with FCU
  //

  if (mode_changing && ((millis() - timeout_timer) >= 30000)) {
    indicateMode(module_config->FLiM);
    mode_changing = false;

    /// per MLCB MNS -- send an NNACK if keeping previous node number > 0
    if (module_config->FLiM && module_config->nodeNum > 0) {
      _msg.len = 3;
      _msg.data[0] = OPC_NNACK;
      _msg.data[1] = highByte(module_config->nodeNum);
      _msg.data[2] = lowByte(module_config->nodeNum);
      sendMessage(&_msg);
    }
  }

  //
  /// end of MLCB message processing
  //

  return;
}

void MLCBbase::check_enumeration(void) {

  //
  /// check the 100ms CAN enumeration cycle timer
  //

  // if (enumeration_active && !enumeration_activeComplete && (millis() - enumeration_start) >= 100) {
  if (enumeration_active && (millis() - enumeration_start) >= 100) {

    byte selected_id = 1;     // default if no responses from other modules

    // enumeration timer has expired -- stop enumeration and process the responses
    // iterate through the 128 bit field
    for (byte i = 0; i < 16; i++) {

      // ignore if this byte is all 1's -> there are no unused IDs in this group of numbers
      if (enumeration_responses[i] == 0xff) {
        continue;
      }

      // for each bit in the byte
      for (byte b = 0; b < 8; b++) {

        // ignore first bit of first byte -- CAN ID zero is not used for nodes
        if (i == 0 && b == 0) {
          continue;
        }

        // if the bit is not set
        if (bitRead(enumeration_responses[i], b) == 0) {
          selected_id = ((i * 16) + b);
          goto check_done;
          break;
        }
      }
    }

check_done:

    // DEBUG_SERIAL << F("> enumeration responses = ") << enums << F(", lowest available CAN id = ") << selected_id << endl;

    // enumeration_activeComplete = true;
    enumeration_active = false;
    enumeration_start = 0UL;

    // store the new CAN ID
    module_config->setCANID(selected_id);

    // send NNACK
    _msg.len = 3;
    _msg.data[0] = OPC_NNACK;
    _msg.data[1] = highByte(module_config->nodeNum);
    _msg.data[2] = lowByte(module_config->nodeNum);
    sendMessage(&_msg);
  }
}

//
/// for accessory event messages, lookup the event in the event table and call the user's registered event handler function
//

void MLCBbase::processAccessoryEvent(unsigned int nn, unsigned int en, bool is_on_event) {

  // try to find a matching stored event -- match on nn, en
  byte index = module_config->findExistingEvent(nn, en);

  // call any registered event handler

  if (index < module_config->EE_MAX_EVENTS) {
    if (eventhandler != NULL) {
      (void)(*eventhandler)(index, & _msg);
    } else if (eventhandlerex != NULL) {
      (void)(*eventhandlerex)(index, & _msg, is_on_event, \
                              ((module_config->EE_NUM_EVS > 0) ? module_config->getEventEVval(index, 1) : 0) \
                             );
    }
  }
}

//
/// set the multipart message handler object to receive multipart message frames
//

void MLCBbase::setMultipartMessageHandler(MLCBMultipartMessage *handler) {
  MultipartMessageHandler = handler;
}

//
/// utility method to populate a MLCB message header
//

void MLCBbase::makeHeader(CANFrame *msg, byte priority) {

  makeHeader_impl(msg, module_config->CANID, priority);
  return;
}

//
/// actual implementation of the makeHeader method
/// so it can be called directly or as a MLCB class method
/// the 11 bit ID of a standard CAN frame is comprised of: (4 bits of MLCB priority) + (7 bits of MLCB CAN ID)
/// priority = 1011 (0xB hex, 11 dec) as default argument, which translates to medium/low
//

void makeHeader_impl(CANFrame *msg, byte id, byte priority) {

  msg->id = (priority << 7) + (id & 0x7f);
  return;
}
