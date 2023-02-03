
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

/// MLCB opcodes

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
