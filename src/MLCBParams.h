/*

  Copyright (C) Sven Rosvall 2021

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

// #define MANU_DEV  13  // For new manufacturer development - who don't have a manufacturer id yet
// #define MANU_MERG 165 // https://www.merg.co.uk
// #define PB_CAN  1 // 
// #define CPUM_ATMEL  2 // 

// #define PF_NOEVENTS 0 // Module doesn't support events
// #define PF_CONSUMER 1 // Module is a consumer of events
// #define PF_PRODUCER 2 // Module is a producer of events
// #define PF_COMBI  3 // Module is both a consumer and producer of events
// #define PF_FLiM 4 // Module is in FLiM
// #define PF_BOOT 8 // Module supports the FCU bootloader protocol
// #define PF_COE  16  // Module can consume its own events
// #define PF_LRN  32  // Module is in learn mode

#include <MLCBdefs.h>
#include <MLCBConfig.h>

class MLCBParams {
public:
  MLCBParams(MLCBConfig const & config) {
    params[0] = 20;                     //  0 num params = 20
    params[1] = MANU_MERG;              //  1 manf = MERG, 165
    params[3] = MTYP_MERGLCB;           //  3 all MLCB modules use same ID
    params[4] = config.EE_MAX_EVENTS;   //  4 num events
    params[5] = config.EE_NUM_EVS;      //  5 num evs per event
    params[6] = config.EE_NUM_NVS;      //  6 num NVs
    params[10] = PB_CAN;                // CAN implementation of MLCB
    params[11] = 0x00;
    params[12] = 0x00;
    params[13] = 0x00;
    params[14] = 0x00;
    initProcessorParams();
  }

  void setVersion(char major, char minor, char build) {
    params[7] = major;                //  7 code major version
    params[2] = minor;                //  2 code minor version
    params[20] = build;                // 20 code beta version
  }

  // // void setModuleId(byte id) {
  // //   params[3] = id;                   //  3 module id
  // // }

  // void setFlags(byte flags) {
  //   params[8] = flags;                //  8 flags - FLiM, consumer/producer
  // }

  // Optional: use this to override processor info that is set by default.
  void setProcessor(byte manufacturer, byte id, char const * name) {
    params[9] = id;                  //  9 processor id
    params[19] = manufacturer;       // 19 processor manufacturer
    memcpy(params + 15, name, 4);   // 15-18 processor version
  }

  unsigned char * getParams() {
    return params;
  }

private:
  // Initializes processor specific parameters based on pre-defined macros in Arduino IDE.
  static void initProcessorParams();

  // Memory for the params is allocated on global memory and handed over to MLCB.setParams().
  static unsigned char params[21];
};

