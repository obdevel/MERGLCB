
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

#define SERVICE_ID_NONE      0xFF
#define SERVICE_ID_ALL       0

//
// MERGLCB Service Types
//
#define SERVICE_ID_MNS      1   ///< The minimum node service. All modules must implement this.
#define SERVICE_ID_NV       2   ///< The NV service.
#define SERVICE_ID_CAN      3   ///< The PIC18 ECAN CAN service. Also implements the CAN transport.
#define SERVICE_ID_TEACH    4   ///< Event teaching service.
#define SERVICE_ID_PRODUCER 5   ///< Event producer service.
#define SERVICE_ID_CONSUMER 6   ///< Event comsumer service.
#define SERVICE_ID_EVENTACK 9   ///< Event acknowledge service. Useful for debugging event configuration.
#define SERVICE_ID_BOOT     10  ///< FCU/PIC bootloader service.

//
/// MANUFACTURER  - Used in the parameter block.
#define MANU_MERG 165
/// MODULE ID    - Used in the parameter block. All MERGLCB modules have the same ID
#define MTYP_MERGLCB    0xFC

//
// Parameters
//
#define PAR_NUM   0 ///< Number of parameters
#define PAR_MANU  1 ///< Manufacturer id
#define PAR_MINVER  2 ///< Minor version letter
#define PAR_MTYP  3 ///< Module type code
#define PAR_EVTNUM  4 ///< Number of events supported
#define PAR_EVNUM 5 ///< Event variables per event
#define PAR_NVNUM 6 ///< Number of Node variables
#define PAR_MAJVER  7 ///< Major version number
#define PAR_FLAGS 8 ///< Node flags
#define PAR_CPUID 9 ///< Processor type
#define PAR_BUSTYPE 10  ///< Bus type
#define PAR_LOAD1 11  ///< load address, 4 bytes
#define PAR_LOAD2 12
#define PAR_LOAD3 13
#define PAR_LOAD4 14
#define PAR_CPUMID  15  ///< CPU manufacturer's id as read from the chip config space, 4 bytes (note - read from cpu at runtime, so not included in checksum)
#define PAR_CPUMAN  19  ///< CPU manufacturer code
#define PAR_BETA  20  ///< Beta revision (numeric), or 0 if release

//
// BUS type that module is connected to
//
#define PB_CAN  1 ///< CAN interface
#define PB_ETH  2 ///< Etrhernet interface
#define PB_MIWI 3 ///< MIWI interface

//
// Error codes for OPC_CMDERR
//
#define CMDERR_INV_CMD          1 ///< Invalid command
#define CMDERR_NOT_LRN          2 ///< Not in learn mode
#define CMDERR_NOT_SETUP        3 ///< Not in setup mode
#define CMDERR_TOO_MANY_EVENTS  4 ///< Too many events
#define CMDERR_NO_EV            5 ///< No EV
#define CMDERR_INV_EV_IDX       6 ///< Invalid EV index
#define CMDERR_INVALID_EVENT  7 ///< Invalid event
#define CMDERR_INV_EN_IDX       8 ///< now reserved
#define CMDERR_INV_PARAM_IDX  9 ///< Invalid param index
#define CMDERR_INV_NV_IDX       10  ///< Invalid NV index
#define CMDERR_INV_EV_VALUE     11  ///< Invalie EV value
#define CMDERR_INV_NV_VALUE     12  ///< Invalid NV value

//
// GRSP codes
//
#define GRSP_OK                 0   ///< Success
#define GRSP_UNKNOWN_NVM_TYPE   254 ///< Unknown non valatile memory type
#define GRSP_INVALID_DIAGNOSTIC 253 ///< Invalid diagnostic
#define GRSP_INVALID_SERVICE    252 ///< Invalid service

//
// Modes
//
#define MODE_UNINITIALISED      0   ///< Uninitialised mode
#define MODE_SETUP      1   ///< Setup mode
#define MODE_NORMAL     2   ///< Normal mode
#define MODE_LEARN      3   ///< Learn mode
#define MODE_EVENT_ACK  4   ///< Event acknowledgement mode
#define MODE_BOOT       5   ///< Boot mode for FCU compatible bootloader
#define MODE_BOOT2      6   ///< Boot mode for MERGLCB boot service
#define MODE_NOHEARTB   7   ///< No heartbeat mode

//
// Processor manufacturer codes
//
#define CPUM_MICROCHIP  1 // 
#define CPUM_ATMEL      2 // 
#define CPUM_ARM        3 // 

//
// Microchip Processor type codes (identifies to FCU for bootload compatiblity)
//
#define P18F2480  1 // 
#define P18F4480  2 // 
#define P18F2580  3 // 
#define P18F4580  4 // 
#define P18F2585  5 // 
#define P18F4585  6 // 
#define P18F2680  7 // 
#define P18F4680  8 // 
#define P18F2682  9 // 
#define P18F4682  10  // 
#define P18F2685  11  // 
#define P18F4685  12  // 
//
#define P18F25K80 13  // 
#define P18F45K80 14  // 
#define P18F26K80 15  // 
#define P18F46K80 16  // 
#define P18F65K80 17  // 
#define P18F66K80 18  // 
#define P18F14K22 19  // 
#define P18F26K83 20  // 
#define P18F27Q84 21  // 
#define P18F47Q84 22  // 
#define P18F27Q83 23  // 
//
#define P32MX534F064  30  // 
#define P32MX564F064  31  // 
#define P32MX564F128  32  // 
#define P32MX575F256  33  // 
#define P32MX575F512  34  // 
#define P32MX764F128  35  // 
#define P32MX775F256  36  // 
#define P32MX775F512  37  // 
#define P32MX795F512  38  // 
//
// ARM Processor type codes (identifies to FCU for bootload compatiblity)
//
#define ARM1176JZF_S  1 // As used in Raspberry Pi
#define ARMCortex_A7  2 // As Used in Raspberry Pi 2
#define ARMCortex_A53 3 // As used in Raspberry Pi 3


/*
 * Message priorities
 */
typedef enum Priority {
  pLOW = 0,
  pNORMAL = 1,
  pABOVE = 2,
  pHIGH = 3,
} Priority;


//
// MERGLCB opcodes list
//
enum {
  OPC_ACK = 0x00,
  OPC_NAK = 0x01,
  OPC_HLT = 0x02,
  OPC_BON = 0x03,
  OPC_TOF = 0x04,
  OPC_TON = 0x05,
  OPC_ESTOP = 0x06,
  OPC_ARST = 0x07,
  OPC_RTOF = 0x08,
  OPC_RTON = 0x09,
  OPC_RESTP = 0x0A,
  OPC_RSTAT = 0x0C,
  OPC_QNN = 0x0D,
  OPC_RQNP = 0x10,
  OPC_RQMN = 0x11,
  OPC_KLOC = 0x21,
  OPC_QLOC = 0x22,
  OPC_DKEEP = 0x23,
  OPC_DBG1 = 0x30,
  OPC_EXTC = 0x3F,
  OPC_RLOC = 0x40,
  OPC_QCON = 0x41,
  OPC_SNN = 0x42,
  OPC_ALOC = 0x43,
  OPC_STMOD = 0x44,
  OPC_PCON = 0x45,
  OPC_KCON = 0x46,
  OPC_DSPD = 0x47,
  OPC_DFLG = 0x48,
  OPC_DFNON = 0x49,
  OPC_DFNOF = 0x4A,
  OPC_SSTAT = 0x4C,
  OPC_NNRSM = 0x4F,
  OPC_RQNN = 0x50,
  OPC_NNREL = 0x51,
  OPC_NNACK = 0x52,
  OPC_NNLRN = 0x53,
  OPC_NNULN = 0x54,
  OPC_NNCLR = 0x55,
  OPC_NNEVN = 0x56,
  OPC_NERD = 0x57,
  OPC_RQEVN = 0x58,
  OPC_WRACK = 0x59,
  OPC_RQDAT = 0x5A,
  OPC_RQDDS = 0x5B,
  OPC_BOOT = 0x5C,
  OPC_ENUM = 0x5D,
  OPC_NNRST = 0x5E,
  OPC_EXTC1 = 0x5F,
  OPC_DFUN = 0x60,
  OPC_GLOC = 0x61,
  OPC_ERR = 0x63,
//    OPC_SQU=0x66,
  OPC_CMDERR = 0x6F,
  OPC_EVNLF = 0x70,
  OPC_NVRD = 0x71,
  OPC_NENRD = 0x72,
  OPC_RQNPN = 0x73,
  OPC_NUMEV = 0x74,
  OPC_CANID = 0x75,
  OPC_MODE = 0x76,
  OPC_RQSD = 0x78,
  OPC_EXTC2 = 0x7F,
  OPC_RDCC3 = 0x80,
  OPC_WCVO = 0x82,
  OPC_WCVB = 0x83,
  OPC_QCVS = 0x84,
  OPC_PCVS = 0x85,
  OPC_RDGN = 0x87,
  OPC_NVSETRD = 0x8E,
  OPC_ACON = 0x90,
  OPC_ACOF = 0x91,
  OPC_AREQ = 0x92,
  OPC_ARON = 0x93,
  OPC_AROF = 0x94,
  OPC_EVULN = 0x95,
  OPC_NVSET = 0x96,
  OPC_NVANS = 0x97,
  OPC_ASON = 0x98,
  OPC_ASOF = 0x99,
  OPC_ASRQ = 0x9A,
  OPC_PARAN = 0x9B,
  OPC_REVAL = 0x9C,
  OPC_ARSON = 0x9D,
  OPC_ARSOF = 0x9E,
  OPC_EXTC3 = 0x9F,
  OPC_RDCC4 = 0xA0,
  OPC_WCVS = 0xA2,
  OPC_SD = 0xAC,
  OPC_HEARTB = 0xAB,
  OPC_GRSP = 0xAF,
  OPC_ACON1 = 0xB0,
  OPC_ACOF1 = 0xB1,
  OPC_REQEV = 0xB2,
  OPC_ARON1 = 0xB3,
  OPC_AROF1 = 0xB4,
  OPC_NEVAL = 0xB5,
  OPC_PNN = 0xB6,
  OPC_ASON1 = 0xB8,
  OPC_ASOF1 = 0xB9,
  OPC_ARSON1 = 0xBD,
  OPC_ARSOF1 = 0xBE,
  OPC_EXTC4 = 0xBF,
  OPC_RDCC5 = 0xC0,
  OPC_WCVOA = 0xC1,
  OPC_CABDAT = 0xC2,
  OPC_DGN = 0xC7,
  OPC_FCLK = 0xCF,
  OPC_ACON2 = 0xD0,
  OPC_ACOF2 = 0xD1,
  OPC_EVLRN = 0xD2,
  OPC_EVANS = 0xD3,
  OPC_ARON2 = 0xD4,
  OPC_AROF2 = 0xD5,
  OPC_ASON2 = 0xD8,
  OPC_ASOF2 = 0xD9,
  OPC_ARSON2 = 0xDD,
  OPC_ARSOF2 = 0xDE,
  OPC_EXTC5 = 0xDF,
  OPC_RDCC6 = 0xE0,
  OPC_PLOC = 0xE1,
  OPC_NAME = 0xE2,
  OPC_STAT = 0xE3,
  OPC_ENACK = 0xE6,
  OPC_DTXC = 0xE9,
  OPC_ESD = 0xE7,
  OPC_PARAMS = 0xEF,
  OPC_ACON3 = 0xF0,
  OPC_ACOF3 = 0xF1,
  OPC_ENRSP = 0xF2,
  OPC_ARON3 = 0xF3,
  OPC_AROF3 = 0xF4,
  OPC_EVLRNI = 0xF5,
  OPC_ACDAT = 0xF6,
  OPC_ARDAT = 0xF7,
  OPC_ASON3 = 0xF8,
  OPC_ASOF3 = 0xF9,
  OPC_DDES = 0xFA,
  OPC_DDRS = 0xFB,
  OPC_ARSON3 = 0xFD,
  OPC_ARSOF3 = 0xFE
};
