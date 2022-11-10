/*
 * Copyright (C) 2005 by Grigoriy A. Sitkarev
 * sitkarev@komi.tgk-9.ru
 *
 * Adapted by Ricardo Olsen from original in http://mrts-ng.googlecode.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __IEC104_TYPES_H
#define __IEC104_TYPES_H

#include <cstdint>

#pragma pack(push)
#pragma pack(1)

struct iec_spe { // start events of protection
  union {
    uint8_t spe;
    struct {
      uint8_t gs : 1;
      uint8_t sl1 : 1;
      uint8_t sl2 : 1;
      uint8_t sl3 : 1;
      uint8_t sie : 1;
      uint8_t srd : 1;
      uint8_t res : 2;
    };
  };
};

struct iec_oci { // Output circuit information of protection equipment
  union {
    uint8_t oci;
    struct {
      uint8_t gc : 1;
      uint8_t cl1 : 1;
      uint8_t cl2 : 1;
      uint8_t cl3 : 1;
      uint8_t res : 4;
    };
  };
};

struct iec_bsi { // Binary state information
  union {
    uint32_t bsi;
    struct {
      uint8_t st1 : 1;
      uint8_t st2 : 1;
      uint8_t st3 : 1;
      uint8_t st4 : 1;
      uint8_t st5 : 1;
      uint8_t st6 : 1;
      uint8_t st7 : 1;
      uint8_t st8 : 1;
      uint8_t st9 : 1;
      uint8_t st10 : 1;
      uint8_t st11 : 1;
      uint8_t st12 : 1;
      uint8_t st13 : 1;
      uint8_t st14 : 1;
      uint8_t st15 : 1;
      uint8_t st16 : 1;
      uint8_t st17 : 1;
      uint8_t st18 : 1;
      uint8_t st19 : 1;
      uint8_t st20 : 1;
      uint8_t st21 : 1;
      uint8_t st22 : 1;
      uint8_t st23 : 1;
      uint8_t st24 : 1;
      uint8_t st25 : 1;
      uint8_t st26 : 1;
      uint8_t st27 : 1;
      uint8_t st28 : 1;
      uint8_t st29 : 1;
      uint8_t st30 : 1;
      uint8_t st31 : 1;
      uint8_t st32 : 1;
    };
  };
};

// 32-bit string state and change data unit
struct iec_stcd {
  union {
    uint16_t  st;
    struct {
      uint8_t st1 : 1;
      uint8_t st2 : 1;
      uint8_t st3 : 1;
      uint8_t st4 : 1;
      uint8_t st5 : 1;
      uint8_t st6 : 1;
      uint8_t st7 : 1;
      uint8_t st8 : 1;
      uint8_t st9 : 1;
      uint8_t st10 : 1;
      uint8_t st11 : 1;
      uint8_t st12 : 1;
      uint8_t st13 : 1;
      uint8_t st14 : 1;
      uint8_t st15 : 1;
      uint8_t st16 : 1;
    };
  };
  union {
    uint16_t  cd;
    struct {
      uint8_t cd1 : 1;
      uint8_t cd2 : 1;
      uint8_t cd3 : 1;
      uint8_t cd4 : 1;
      uint8_t cd5 : 1;
      uint8_t cd6 : 1;
      uint8_t cd7 : 1;
      uint8_t cd8 : 1;
      uint8_t cd9 : 1;
      uint8_t cd10 : 1;
      uint8_t cd11 : 1;
      uint8_t cd12 : 1;
      uint8_t cd13 : 1;
      uint8_t cd14 : 1;
      uint8_t cd15 : 1;
      uint8_t cd16 : 1;
    };
  };
};

// CP56Time2a timestamp
struct cp56time2a {
  uint16_t msec;
  uint8_t min : 6;
  uint8_t res1 : 1;
  uint8_t iv : 1;
  uint8_t hour : 5;
  uint8_t res2 : 2;
  uint8_t su : 1;
  uint8_t mday : 5;
  uint8_t wday : 3;
  uint8_t month : 4;
  uint8_t res3 : 4;
  uint8_t year : 7;
  uint8_t res4 : 1;
};

// CP16Time2a milliseconds time
typedef struct cp56time2a cp56time2a;

struct cp16time2a {
  uint16_t milliseconds;
};

typedef  cp16time2a cp16Time2a;

struct iec_qualif {
  uint8_t var : 2;
  uint8_t res : 2;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
};

// M_SP_NA_1 - single point information with quality description
struct iec_type1 {
  uint8_t sp : 1; // single point information
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
};

// M_DP_NA_1 - double point information with quality description
struct iec_type3 {
  uint8_t dp : 2; // double point information
  uint8_t res : 2;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
};

// M_ST_NA_1 - step position
struct iec_type5 {
  uint8_t mv  : 7; // value
  uint8_t t : 1; // transient flag
  uint8_t ov : 1; // overflow/no overflow
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
};

// M_BO_NA_1 - state and change information bit string
struct iec_type7 {
  iec_bsi bsi;
  uint8_t ov : 1; // overflow/no overflow
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
};

// M_ME_NA_1 - normalized measured value
struct iec_type9 {
  int16_t mv; // normalized value
  uint8_t ov : 1; // overflow/no overflow
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
};

// M_ME_NB_1 - scaled measured value
struct iec_type11 {
  int16_t mv; // scaled value
  uint8_t ov : 1; // overflow/no overflow
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
};

// M_ME_NC_1 - short floating point measured value
struct iec_type13 {
  float mv; // float value
  uint8_t ov : 1; // overflow/no overflow
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
};

// M_ME_NC_1 - integrated totals
struct iec_type15 {
  uint32_t bcr;
  uint8_t sq : 5; // sequence
  uint8_t cy : 1; // carry
  uint8_t ca : 1; // counter adjusted
  uint8_t iv : 1; // valid/invalid
};

// M_PS_NA_1 - Packed single point information with status change detection
struct iec_type20 {
  struct iec_stcd stcd;
  uint8_t ov : 1; // overflow/no overflow
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
};

// M_ME_ND_1 - normalized measured value without quality descriptor
struct iec_type21 {
  int16_t mv; // normalized value
};

// M_SP_TB_1 - single point information with quality description and time tag
struct iec_type30 {
  uint8_t sp : 1; // single point information
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
  cp56time2a time;
};

// M_DP_TB_1 - double point information with quality description and time tag
struct iec_type31 {
  uint8_t dp : 2; // double point information
  uint8_t res : 2;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
  cp56time2a time;
};

// M_ST_TB_1 - step position with time tag
struct iec_type32 {
  uint8_t mv  : 7; // value
  uint8_t t : 1; // transient flag
  uint8_t ov : 1; // overflow/no overflow
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
  cp56time2a time;
};

// M_BO_TB_1 - state and change information bit string and time tag
struct iec_type33 {
  iec_bsi bsi;
  uint8_t ov : 1; // overflow/no overflow
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
  cp56time2a time;
};

// M_ME_TD_1 - normalized measured value with time tag
struct iec_type34 {
  int16_t mv; // normalized value
  uint8_t ov : 1; // overflow/no overflow
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
  cp56time2a time;
};

// M_ME_TE_1 - scaled measured value with time tag
struct iec_type35 {
  int16_t mv; // scaled value
  uint8_t ov : 1; // overflow/no overflow
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
  cp56time2a time;
};

// M_ME_TF_1 - short floating point measurement value and time tag
struct iec_type36 {
  float  mv;
  uint8_t ov : 1; // overflow/no overflow
  uint8_t res : 3;
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
  cp56time2a time;
};

// M_IT_TB_1 - Integrated totals with time tag CP56Time2a
struct iec_type37 {
  uint32_t bcr;
  unsigned char sq : 5;
  unsigned char cy : 1;
  unsigned char ca : 1;
  unsigned char iv : 1;
  cp56time2a time;
};

// M_EP_TD_1 - Event of protection equipment with CP56Time2a time tag
struct iec_type38 {
  uint8_t es : 2; // event state
  uint8_t res : 1; // reserved
  uint8_t ei : 1; // elapsed time not valid/valid (1-invalid)
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
  cp16time2a elapsed;
  cp56time2a time;
};

// M_EP_TE_1 - Packed start events of protection equipment with CP56Time2a time tag
struct iec_type39 {
  iec_spe spe; // SPE = Start events of protection equipment
  uint8_t res : 3; // reserved
  uint8_t ei : 1; // elapsed time not valid/valid (1-invalid)
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
  cp16time2a elapsed;
  cp56time2a time;
};

// M_EP_TF_1 - Packed output circuit information of protection equipment with CP56Time2a time tag
struct iec_type40 {
  iec_oci oci; // OCI = Output cirquit information of protection equipment
  uint8_t res : 3; // reserved
  uint8_t ei : 1; // elapsed time not valid/valid (1-invalid)
  uint8_t bl : 1; // blocked/not blocked
  uint8_t sb : 1; // substituted/not substituted
  uint8_t nt : 1; // not topical/topical
  uint8_t iv : 1; // valid/invalid
  cp16time2a elapsed;
  cp56time2a time;
};

// C_SC_NA_1 - Single command
struct iec_type45 {
  uint8_t scs : 1; // single command state
  uint8_t res : 1; // must be zero
  uint8_t qu : 5;
  uint8_t se : 1; // select=1 / execute=0
};

// C_DC_NA_1 - Double command
struct iec_type46 {
  uint8_t dcs : 2; // double command state
  uint8_t qu : 5;
  uint8_t se : 1; // select=1 / execute=0
};

// C_RC_NA_1 - Regulating step command
struct iec_type47 {
  uint8_t rcs : 2; // regulating step command
  uint8_t qu : 5;
  uint8_t se : 1; // select=1 / execute=0
};

// C_SE_NA_1 - Set-point Command, normalised value
struct iec_type48 {
  int16_t nva; // normalised value
  uint8_t ql : 7; // command qualifier
  uint8_t se : 1; // select=1 / execute=0
};

// C_SE_TA_1 - Normalised value command with time tag CP56Time2a
struct iec_type61 {
  int16_t nva; // normalised value
  uint8_t ql : 7; // command qualifier
  uint8_t se : 1; // select=1 / execute=0
  cp56time2a time;
};

// C_SE_NB_1 - Set-point Command, scaled value
struct iec_type49 {
  int16_t sva; // scaled value
  uint8_t ql : 7; // command qualifier
  uint8_t se : 1; // select=1 / execute=0
};

// C_SE_TB_1 - Scaled value command with time tag CP56Time2a
struct iec_type62 {
  int16_t sva; // scaled value
  uint8_t ql : 7; // command qualifier
  uint8_t se : 1; // select=1 / execute=0
  cp56time2a time;
};

// C_SE_NC_1 - Set-point Command, short floating point value
struct iec_type50 {
  float r32; // float value
  uint8_t ql : 7; // command qualifier
  uint8_t se : 1; // select=1 / execute=0
};

// C_SE_TC_1 - Short floating point number command with time tag CP56Time2a
struct iec_type63 {
  float r32; // float value
  uint8_t ql : 7; // command qualifier
  uint8_t se : 1; // select=1 / execute=0
  cp56time2a time;
};

// C_SC_TA_1 - Single command with time tag CP56Time2a
struct iec_type58 {
  uint8_t scs : 1; // single command state
  uint8_t res : 1; // must be zero
  uint8_t qu : 5;
  uint8_t se : 1; // select=1 / execute=0
  cp56time2a time;
};

// C_DC_TA_1 - Double command with time tag CP56Time2a
struct iec_type59 {
  uint8_t dcs : 2; // double command state
  uint8_t qu : 5;
  uint8_t se  : 1; // select=1 / execute=0
  cp56time2a time;
};

// C_RC_TA_1 - Regulating step command with time tag CP56Time2a
struct iec_type60 {
  uint8_t rcs : 2; // regulating step command
  uint8_t qu : 5;
  uint8_t se  : 1; // select=1 / execute=0
  cp56time2a time;
};

struct iec_type100 {
  uint8_t qoi; // pointer of interrogation
};

struct iec_type101 {
  uint16_t ioa16; // object address bytes 1,2
  uint8_t ioa8; // object address byte 3
  uint8_t rqt : 6; // request
  uint8_t frz : 2; // freeze
};

struct iec_type102 {
  uint16_t ioa16; // object address bytes 1,2
  uint8_t ioa8; // object address byte 3
};

struct iec_type103 {
  uint16_t ioa16; // object address bytes 1,2
  uint8_t ioa8; // object address byte 3
  cp56time2a time;
};

struct iec_type105 {
  uint8_t qrp; // QRP qualifier of reset process
};

struct iec_type107 {
  uint16_t ioa16; // object address bytes 1,2
  uint8_t ioa8; // object address byte 3
  uint16_t tsc; // TSC test sequence counter
  cp56time2a time;
};

// P_ME_NA_1 - Parameter of measured value, normalized value.
struct iec_type110 {
  int16_t nva; // normalized value
  uint8_t kpa : 6; // Kind of parameter. 0=unused, 1=thresh, 2=filter, 3=lolimit, 4=hilimit
  uint8_t pop : 1; // pop?
  uint8_t lpc : 1; // Local parameter change flag: no change=0, changed=1
};

// P_ME_NB_1 - Parameter of measured value, scaled value.
struct iec_type111 {
  int16_t sva; // scaled value
  uint8_t kpa : 6; // Kind of parameter. 0=unused, 1=thresh, 2=filter, 3=lolimit, 4=hilimit
  uint8_t pop : 1; // pop?
  uint8_t lpc : 1; // Local parameter change flag: no change=0, changed=1
};

// P_ME_NC_1 - Parameter of measured value, short floating point value.
struct iec_type112 {
  float r32; // float value
  uint8_t kpa : 6; // Kind of parameter. 0=unused, 1=thresh, 2=filter, 3=lolimit, 4=hilimit
  uint8_t pop : 1; // pop?
  uint8_t lpc : 1; // Local parameter change flag: no change=0, changed=1
};

// P_AC_NA_1 - Parameter activation.
struct iec_type113 {
  uint8_t qpa; // Qualifier of parameter activation. 0=unused, 1=general, 2=object, 3=transmission
};

// Data unit identifier block - ASDU header
struct iec_unit_id {
  uint8_t type;  // type identification
  uint8_t num : 7; // number of information objects
  uint8_t sq : 1; // sequenced/not sequenced address
  uint8_t cause : 6; // cause of transmission
  uint8_t pn : 1; // positive/negative app. confirmation
  uint8_t t : 1; // test
  uint8_t oa; // originator addres
  uint16_t ca; // common address of ASDU
};

struct iec_apdu {
  uint8_t start;
  uint8_t length;
  uint16_t NS;
  uint16_t NR;
  struct iec_unit_id asduh;
  union {
    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type1 obj[1];
    } sq1;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type1 obj;
    } nsq1[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type3 obj[1];
    } sq3;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type3 obj;
    } nsq3[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type5 obj[1];
    } sq5;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type5 obj;
    } nsq5[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type7 obj[1];
    } sq7;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type7 obj;
    } nsq7[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type9 obj[1];
    } sq9;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type9 obj;
    } nsq9[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type11 obj[1];
    } sq11;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type11 obj;
    } nsq11[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type13 obj[1];
    } sq13;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type13 obj;
    } nsq13[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type15 obj[1];
    } sq15;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type15 obj;
    } nsq15[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type20 obj[1];
    } sq20;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type20 obj;
    } nsq20[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type21 obj[1];
    } sq21;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type21 obj;
    } nsq21[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type30 obj[1];
    } sq30;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type30 obj;
    } nsq30[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type31 obj[1];
    } sq31;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type31 obj;
    } nsq31[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type32 obj[1];
    } sq32;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type32 obj;
    } nsq32[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type33 obj[1];
    } sq33;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type33 obj;
    } nsq33[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type34 obj[1];
    } sq34;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type34 obj;
    } nsq34[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type35 obj[1];
    } sq35;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type35 obj;
    } nsq35[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type36 obj[1];
    } sq36;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type36 obj;
    } nsq36[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type37 obj[1];
    } sq37;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type37 obj;
    } nsq37[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type38 obj[1];
    } sq38;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type38 obj;
    } nsq38[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type39 obj[1];
    } sq39;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type39 obj;
    } nsq39[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type40 obj[1];
    } sq40;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type40 obj;
    } nsq40[1];

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type45 obj;
    } nsq45;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type46 obj;
    } nsq46;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type47 obj;
    } nsq47;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type48 obj;
    } nsq48;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type49 obj;
    } nsq49;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type50 obj;
    } nsq50;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type61 obj;
    } nsq61;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type62 obj;
    } nsq62;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type63 obj;
    } nsq63;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type58 obj;
    } nsq58;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type59 obj;
    } nsq59;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type60 obj;
    } nsq60;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type110 obj;
    } nsq110;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type111 obj;
    } nsq111;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type112 obj;
    } nsq112;

    struct {
      uint16_t ioa16;
      uint8_t ioa8;
      iec_type113 obj;
    } nsq113;

    uint8_t dados[255];
    iec_type101 asdu101;
    iec_type102 asdu102;
    iec_type103 asdu103;
    iec_type105 asdu105;
    iec_type107 asdu107;
  };
};

#pragma pack(pop)

#endif // __IEC104_TYPES_H
