
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

#pragma once
#include <Arduino.h>

#ifndef DEBUG_SERIAL
#define DEBUG_SERIAL Serial
#endif

#include <SPI.h>

#include <MLCBLED.h>
#include <MLCBSwitch.h>
#include <MLCBConfig.h>
#include <MLCBdefs.h>

#define SW_TR_HOLD 8000U                           // MLCB push button hold time for SLiM/FLiM transition in millis = 8 seconds
#define DEFAULT_PRIORITY 0xB                       // default MLCB messages priority. 1011 = 2|3 = normal/low
#define MULTIPART_MESSAGE_DEFAULT_DELAY 20U        // delay in milliseconds between sending successive long message fragments
#define MULTIPART_MESSAGE_RECEIVE_TIMEOUT 5000UL   // timeout waiting for next long message packet
#define NUM_EX_CONTEXTS 4U                         // number of send and receive contexts for extended implementation = number of concurrent messages
#define EX_BUFFER_LEN 64U                          // size of extended send and receive buffers
#define HBTIMER_INTERVAL 5000UL                    // heartbeat interval in ms 

//
/// MLCB modes
//

enum {
  MODE_SLIM = 0,
  MODE_FLIM = 1,
  MODE_CHANGING = 2
};

//
/// MLCB long message status codes
//

enum {
  MLCB_MULTIPART_MESSAGE_INCOMPLETE = 0,
  MLCB_MULTIPART_MESSAGE_COMPLETE,
  MLCB_MULTIPART_MESSAGE_SEQUENCE_ERROR,
  MLCB_MULTIPART_MESSAGE_TIMEOUT_ERROR,
  MLCB_MULTIPART_MESSAGE_CRC_ERROR,
  MLCB_MULTIPART_MESSAGE_TRUNCATED
};

//
/// CAN/MLCB message type
//

class CANFrame {

public:
  uint32_t id;
  bool ext;
  bool rtr;
  uint8_t len;
  uint8_t data[8] = {};
};

//
/// an abstract class to encapsulate CAN bus and MLCB processing
/// it must be implemented by a derived subclass
//

class MLCBMultipartMessage;      // forward reference

class MLCBbase {

public:
  MLCBbase(MLCBConfig *the_config);

  // these methods are pure virtual and must be implemented by the derived class
  // as a consequence, it is not possible to create an instance of this class

#ifdef ARDUINO_ARCH_RP2040
  virtual bool begin(bool poll = false, SPIClassRP2040 spi = SPI) = 0;
#else
  virtual bool begin(bool poll = false, SPIClass spi = SPI) = 0;
#endif
  virtual bool available(void) = 0;
  virtual CANFrame getNextMessage(void) = 0;
  virtual bool sendMessage(CANFrame *msg, bool rtr = false, bool ext = false, byte priority = DEFAULT_PRIORITY) = 0;
  virtual void reset(void) = 0;

  // implementations of these methods are provided in the base class

  bool sendWRACK(void);
  bool sendCMDERR(byte cerrno);
  bool sendGRSP(byte cerrno, byte opcode = 0, byte data1 = 0, byte data2 = 0);
  void start_enumeration(void);
  void check_enumeration(void);
  byte getCANID(unsigned long header);
  bool isExt(CANFrame *msg);
  bool isRTR(CANFrame *msg);
  void process(byte num_messages = 3);
  void initFLiM(void);
  void revertSLiM(void);
  void setSLiM(void);
  void renegotiate(void);
  void setLEDs(MLCBLED ledGrn, MLCBLED ledYlw);
  void setSwitch(MLCBSwitch sw);
  void setParams(unsigned char *mparams);
  void setName(unsigned char *mname);
  void indicateMode(byte mode);
  void setEventHandler(void (*fptr)(byte index, CANFrame *msg));
  void setEventHandler(void (*fptr)(byte index, CANFrame *msg, bool ison, byte evval));
  void setFrameHandler(void (*fptr)(CANFrame *msg), byte *opcodes = NULL, byte num_opcodes = 0);
  void makeHeader(CANFrame *msg, byte priority = DEFAULT_PRIORITY);
  void processAccessoryEvent(unsigned int nn, unsigned int en, bool is_on_event);
  void setMultipartMessageHandler(MLCBMultipartMessage *handler);

  unsigned int _numMsgsSent, _numMsgsRcvd, _numMsgsActioned, _numNNchanges;

protected:                                          // protected members become private in derived classes
  CANFrame _msg;
  MLCBLED _ledGrn, _ledYlw;
  MLCBSwitch _sw;
  MLCBConfig *module_config;
  unsigned char *_mparams;
  unsigned char *_mname;
  void (*eventhandler)(byte index, CANFrame *msg);
  void (*eventhandlerex)(byte index, CANFrame *msg, bool evOn, byte evVal);
  void (*framehandler)(CANFrame *msg);
  byte *_opcodes;
  byte _num_opcodes;
  byte enumeration_responses[16];                          // 128 bits for storing CAN ID enumeration results
  bool mode_changing, enumeration_active, bLearn;
  unsigned long timeout_timer, enumeration_start;
  bool enumeration_required;
  bool UI = false;
  bool isMLCB = false;
  bool hbactive;
  uint8_t hbcount;
  uint32_t hbtimer;

  MLCBMultipartMessage *MultipartMessageHandler = NULL;       // MLCB long message object to receive relevant frames
};

//
/// a basic class to send and receive MLCB long messages per MERG RFC 0005
/// handles a single message, sending and receiving
/// suitable for small microcontrollers with limited memory
//

class MLCBMultipartMessage {

public:

  MLCBMultipartMessage(MLCBbase *MLCB_object_ptr);
  bool sendMultipartMessage(const void *msg, const unsigned int msg_len, const byte stream_id, const byte priority = DEFAULT_PRIORITY);
  void subscribe(byte *stream_ids, const byte num_stream_ids, void *receive_buffer, const unsigned int receive_buffer_len, void (*messagehandler)(void *fragment, const unsigned int fragment_len, const byte stream_id, const byte status));
  bool process(void);
  virtual void processReceivedMessageFragment(const CANFrame *frame);
  bool is_sending(void);
  void setDelay(byte delay_in_millis);
  void setTimeout(unsigned int timeout_in_millis);

protected:

  bool sendMessageFragment(CANFrame *frame, const byte priority);

  bool _is_receiving = false;
  byte *_send_buffer, *_receive_buffer;
  byte _send_stream_id = 0, _receive_stream_id = 0, *_stream_ids = NULL, _num_stream_ids = 0, _send_priority = DEFAULT_PRIORITY, _msg_delay = MULTIPART_MESSAGE_DEFAULT_DELAY, _sender_canid = 0;
  unsigned int _send_buffer_len = 0, _incoming_message_length = 0, _receive_buffer_len = 0, _receive_buffer_index = 0, _send_buffer_index = 0, _incoming_message_crc = 0, \
                                  _incoming_bytes_received = 0, _receive_timeout = MULTIPART_MESSAGE_RECEIVE_TIMEOUT, _send_sequence_num = 0, _expected_next_receive_sequence_num = 0;
  unsigned long _last_fragment_sent = 0UL, _last_fragment_received = 0UL;

  void (*_messagehandler)(void *fragment, const unsigned int fragment_len, const byte stream_id, const byte status);        // user callback function to receive long message fragments
  MLCBbase *_MLCB_object_ptr;
};


//// extended support for multiple concurrent long messages

// send and receive contexts

typedef struct _receive_context_t {
  bool in_use;
  byte receive_stream_id, sender_canid;
  byte *buffer;
  unsigned int receive_buffer_index, incoming_bytes_received, incoming_message_length, expected_next_receive_sequence_num, incoming_message_crc;
  unsigned long last_fragment_received;
} receive_context_t;

typedef struct _send_context_t {
  bool in_use;
  byte send_stream_id, send_priority, msg_delay;
  byte *buffer;
  unsigned int send_buffer_len, send_buffer_index, send_sequence_num;
  unsigned long last_fragment_sent;
} send_context_t;

//
/// a derived class to extend the base long message class to handle multiple concurrent messages, sending and receiving
//

class MLCBMultipartMessageEx : public MLCBMultipartMessage {

public:

  MLCBMultipartMessageEx(MLCBbase *MLCB_object_ptr)
    : MLCBMultipartMessage(MLCB_object_ptr) {}         // derived class constructor calls the base class constructor

  bool allocateContexts(byte num_receive_contexts = NUM_EX_CONTEXTS, unsigned int receive_buffer_len = EX_BUFFER_LEN, byte num_send_contexts = NUM_EX_CONTEXTS);
  bool sendMultipartMessage(const void *msg, const unsigned int msg_len, const byte stream_id, const byte priority = DEFAULT_PRIORITY);
  bool process(void);
  void subscribe(byte *stream_ids, const byte num_stream_ids, void (*messagehandler)(void *msg, unsigned int msg_len, byte stream_id, byte status));
  virtual void processReceivedMessageFragment(const CANFrame *frame);
  byte is_sending(void);
  void use_crc(bool use_crc);

private:

  bool _use_crc = false;
  byte _num_receive_contexts = NUM_EX_CONTEXTS, _num_send_contexts = NUM_EX_CONTEXTS;
  receive_context_t **_receive_context = NULL;
  send_context_t **_send_context = NULL;
};

