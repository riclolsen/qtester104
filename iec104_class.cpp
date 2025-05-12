/*
 * This software implements an IEC 60870-5-104 protocol tester.
 * Copyright © 2010-2024 Ricardo L. Olsen
 *
 * Disclaimer
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <string>
#include <sstream>

#include "iec104_class.h"

using namespace std;

iec104_class::iec104_class() {
  strncpy(slaveIP, "", 20);

  mapTiStr[0] = "M_UNDEF";
  mapTiStr[1] = "M_SP_NA_1";
  mapTiStr[2] = "M_SP_TA_1";
  mapTiStr[3] = "M_DP_NA_1";
  mapTiStr[4] = "M_DP_TA_1";
  mapTiStr[5] = "M_ST_NA_1";
  mapTiStr[6] = "M_ST_TA_1";
  mapTiStr[7] = "M_BO_NA_1";
  mapTiStr[8] = "M_BO_TA_1";
  mapTiStr[9] = "M_ME_NA_1";
  mapTiStr[10] = "M_ME_TA_1";
  mapTiStr[11] = "M_ME_NB_1";
  mapTiStr[12] = "M_ME_TB_1";
  mapTiStr[13] = "M_ME_NC_1";
  mapTiStr[14] = "M_ME_TC_1";
  mapTiStr[15] = "M_IT_NA_1";
  mapTiStr[16] = "M_IT_TA_1";
  mapTiStr[17] = "M_EP_TA_1";
  mapTiStr[18] = "M_EP_TB_1";
  mapTiStr[19] = "M_EP_TC_1";
  mapTiStr[20] = "M_PS_NA_1";
  mapTiStr[21] = "M_ME_ND_1";
  mapTiStr[30] = "M_SP_TB_1";
  mapTiStr[31] = "M_DP_TB_1";
  mapTiStr[32] = "M_ST_TB_1";
  mapTiStr[33] = "M_BO_TB_1";
  mapTiStr[34] = "M_ME_TD_1";
  mapTiStr[35] = "M_ME_TE_1";
  mapTiStr[36] = "M_ME_TF_1";
  mapTiStr[37] = "M_IT_TB_1";
  mapTiStr[38] = "M_EP_TD_1";
  mapTiStr[39] = "M_EP_TE_1";
  mapTiStr[40] = "M_EP_TF_1";
  mapTiStr[45] = "C_SC_NA_1";
  mapTiStr[46] = "C_DC_NA_1";
  mapTiStr[47] = "C_RC_NA_1";
  mapTiStr[48] = "C_SE_NA_1";
  mapTiStr[49] = "C_SE_NB_1";
  mapTiStr[50] = "C_SE_NC_1";
  mapTiStr[51] = "C_BO_NA_1";
  mapTiStr[58] = "C_SC_TA_1";
  mapTiStr[59] = "C_DC_TA_1";
  mapTiStr[60] = "C_RC_TA_1";
  mapTiStr[61] = "C_SE_TA_1";
  mapTiStr[62] = "C_SE_TB_1";
  mapTiStr[63] = "C_SE_TC_1";
  mapTiStr[64] = "C_BO_TA_1";
  mapTiStr[70] = "M_EI_NA_1";
  mapTiStr[100] = "C_IC_NA_1";
  mapTiStr[101] = "C_CI_NA_1";
  mapTiStr[102] = "C_RD_NA_1";
  mapTiStr[103] = "C_CS_NA_1";
  mapTiStr[104] = "C_TS_NA_1";
  mapTiStr[105] = "C_RP_NA_1";
  mapTiStr[106] = "C_CD_NA_1";
  mapTiStr[107] = "C_TS_TA_1";
  mapTiStr[110] = "P_ME_NA_1";
  mapTiStr[111] = "P_ME_NB_1";
  mapTiStr[112] = "P_ME_NC_1";
  mapTiStr[113] = "P_AC_NA_1";
  mapTiStr[120] = "F_FR_NA_1";
  mapTiStr[121] = "F_SR_NA_1";
  mapTiStr[122] = "F_SC_NA_1";
  mapTiStr[123] = "F_LS_NA_1";
  mapTiStr[124] = "F_FA_NA_1";
  mapTiStr[125] = "F_SG_NA_1";
  mapTiStr[126] = "F_DR_TA_1";

  mapCauseStr[0] = "UNUSED";
  mapCauseStr[1] = "CYCLIC";
  mapCauseStr[2] = "BACKGND";
  mapCauseStr[3] = "SPONT";
  mapCauseStr[4] = "INIT";
  mapCauseStr[5] = "REQ";
  mapCauseStr[6] = "ACT";
  mapCauseStr[7] = "ACT_CON";
  mapCauseStr[8] = "DEACT";
  mapCauseStr[9] = "DEACT_CON";
  mapCauseStr[10] = "ACT_TERM";
  mapCauseStr[11] = "RETREM";
  mapCauseStr[12] = "RETLOC";
  mapCauseStr[13] = "FILE";
  mapCauseStr[14] = "COT_14";
  mapCauseStr[15] = "COT_15";
  mapCauseStr[16] = "COT_16";
  mapCauseStr[17] = "COT_17";
  mapCauseStr[18] = "COT_18";
  mapCauseStr[19] = "COT_19";
  mapCauseStr[20] = "INROGEN";
  mapCauseStr[21] = "INRO1";
  mapCauseStr[22] = "INRO2";
  mapCauseStr[23] = "INRO3";
  mapCauseStr[24] = "INRO4";
  mapCauseStr[25] = "INRO5";
  mapCauseStr[26] = "INRO6";
  mapCauseStr[27] = "INRO7";
  mapCauseStr[28] = "INRO8";
  mapCauseStr[29] = "INRO9";
  mapCauseStr[30] = "INRO10";
  mapCauseStr[31] = "INRO11";
  mapCauseStr[32] = "INRO12";
  mapCauseStr[33] = "INRO13";
  mapCauseStr[34] = "INRO14";
  mapCauseStr[35] = "INRO15";
  mapCauseStr[36] = "INRO16";
  mapCauseStr[37] = "REQCOGEN";
  mapCauseStr[38] = "REQCO1";
  mapCauseStr[39] = "REQCO2";
  mapCauseStr[40] = "REQCO3";
  mapCauseStr[41] = "REQCO4";
  mapCauseStr[42] = "COT_42";
  mapCauseStr[43] = "COT_43";
  mapCauseStr[44] = "UNKNOWN_TYPE";
  mapCauseStr[45] = "UNKNOWN_CAUSE";
  mapCauseStr[46] = "UNKNOWN_ASDU_ADDR";
  mapCauseStr[47] = "UNKNOWN_OBJ_ADDR";

  Port = 2404;

  msg_supervisory = true;
  seq_order_check = true;
  connectedTCP = false;

  tout_startdtact = -1;
  tout_supervisory = -1;
  tout_testfr = -1;
  tout_gi = -1;
  VS = 0;
  VR = 0;
  TxOk = false;
  masterAddress = 0;
  slaveAddress = 0;
  slaveASDUAddrCmd = 0;
  GIObjectCnt = 0;
  gi_period = 5 * 60 + 30;
}

string iec104_class::asduTiStr(int ti) {
  string s = "";
  map<int, string>::const_iterator iter = mapTiStr.find(ti);
  if (iter != mapTiStr.end()) {
    s = iter->second;
  }
  if (s == "" && ti >= 127)
    s = "USER_RESERVED";
  else if (s == "" && ti < 127)
    s = "STD_RESERVED";
  return s;
}

string iec104_class::causeStr(int cause) {
  string s = "COT_?";
  map<int, string>::const_iterator iter = mapCauseStr.find(cause);
  if (iter != mapCauseStr.end()) {
    s = iter->second;
  }
  return s;
}

void iec104_class::disableSequenceOrderCheck() {
  seq_order_check = false;
}

int iec104_class::getPortTCP() {
  return int(Port);
}

void iec104_class::setGIPeriod(unsigned period) {
  gi_period = int(period);
}

void iec104_class::setPortTCP(unsigned port) {
  Port = port;
}

void iec104_class::setSecondaryIP(char* ip) {
  strncpy(slaveIP, ip, 20);
}

void iec104_class::setSecondaryIP_backup(char* ip) {
  strncpy(slaveIP_backup, ip, 20);
}

char* iec104_class::getSecondaryIP() {
  return slaveIP;
}

char* iec104_class::getSecondaryIP_backup() {
  return slaveIP_backup;
}

void iec104_class::setSecondaryAddress(int addr) {
  slaveAddress = static_cast<unsigned short>(addr);
}

void iec104_class::setSecondaryASDUAddress(int addr) {
  slaveASDUAddrCmd = static_cast<unsigned short>(addr);
}

int iec104_class::getSecondaryAddress() {
  return slaveAddress;
}

void iec104_class::setPrimaryAddress(int addr) {
  masterAddress = static_cast<unsigned char>(addr);
}

int iec104_class::getPrimaryAddress() {
  return masterAddress;
}

void iec104_class::onConnectTCP() {
  connectedTCP = true;
  TxOk = false;
  VS = 0;
  VR = 0;
  test_command_count = 0;
  mLog.pushMsg("*** TCP CONNECT!");
  sendStartDTACT();
}

void iec104_class::onDisconnectTCP() {
  connectedTCP = false;
  tout_startdtact = -1;
  tout_supervisory = -1;
  tout_gi = -1;
  TxOk = false;
  mLog.pushMsg("*** TCP DISCONNECT!");
}

void iec104_class::onTimerSecond() {
  iec_apdu apdu;

  if (connectedTCP) {
    if (tout_startdtact > 0)
      tout_startdtact--;
    if (tout_startdtact == 0)  // timeout of startdtact: retry
      sendStartDTACT();

    if (tout_gi > 0) {
      tout_gi--;
      if (tout_gi == 0) {
        solicitGI();
      }
    }

    if (msg_supervisory) {
      if (tout_supervisory > 0)
        tout_supervisory--;
      if (tout_supervisory > 0)
        tout_supervisory--;

      if (tout_supervisory == 0) {
        tout_supervisory = -1;
        sendSupervisory();
      }
    }
  }

  // if connected and no data received, send TESTFRACT
  if (connectedTCP && TxOk) {
    if (tout_testfr > 0) {
      tout_testfr--;
      if (tout_testfr == 0) {
        apdu.start = START;
        apdu.length = 4;
        apdu.NS = TESTFRACT;
        apdu.NR = 0;
        sendTCP(reinterpret_cast<char*>(&apdu), 6);
        mLog.pushMsg("     TESTFRACT");
      }
    }
  }

}

void iec104_class::solicitGI() {
  iec_apdu wapdu;

  wapdu.start = START;
  wapdu.length = 0x0E;
  wapdu.NS = VS;
  wapdu.NR = VR;
  wapdu.asduh.type = INTERROGATION;
  wapdu.asduh.num = 1;
  wapdu.asduh.sq = 0;
  wapdu.asduh.cause = ACTIVATION;
  wapdu.asduh.t = 0;
  wapdu.asduh.pn = 0;
  wapdu.asduh.oa = masterAddress;
  wapdu.asduh.ca = slaveAddress;
  wapdu.dados[0] = 0x00;
  wapdu.dados[1] = 0x00;
  wapdu.dados[2] = 0x00;
  wapdu.dados[3] = 0x14;
  sendTCP(reinterpret_cast<char*>(&wapdu), 16);
  VS += 2;
  mLog.pushMsg("    GENERAL INTERROGATION ");
  tout_gi = gi_retry_time;
}

void iec104_class::solicitInterrogation(char group) {
  iec_apdu wapdu;

  wapdu.start = START;
  wapdu.length = 0x0E;
  wapdu.NS = VS;
  wapdu.NR = VR;
  wapdu.asduh.type = INTERROGATION;
  wapdu.asduh.num = 1;
  wapdu.asduh.sq = 0;
  wapdu.asduh.cause = ACTIVATION;
  wapdu.asduh.t = 0;
  wapdu.asduh.pn = 0;
  wapdu.asduh.oa = masterAddress;
  wapdu.asduh.ca = slaveAddress;
  wapdu.dados[0] = 0x00;
  wapdu.dados[1] = 0x00;
  wapdu.dados[2] = 0x00;
  wapdu.dados[3] = group;
  sendTCP(reinterpret_cast<char*>(&wapdu), 16);
  VS += 2;
  char buflog[1000];
  sprintf(buflog, "     INTERROGATION GROUP %d", group);
  mLog.pushMsg(buflog);
  tout_gi = gi_retry_time;
}

void iec104_class::confTestCommand() {
  iec_apdu wapdu;

  wapdu.start = START;
  wapdu.length = 22;
  wapdu.NS = VS;
  wapdu.NR = VR;
  wapdu.asduh.type = C_TS_TA_1;
  wapdu.asduh.num = 1;
  wapdu.asduh.sq = 0;
  wapdu.asduh.cause = ACTCONFIRM;
  wapdu.asduh.t = 0;
  wapdu.asduh.pn = 0;
  wapdu.asduh.oa = masterAddress;
  wapdu.asduh.ca = slaveAddress;
  time_t tm1 = time(nullptr);
  tm* agora = localtime(&tm1);

  wapdu.asdu107.ioa16 = 0;
  wapdu.asdu107.ioa8 = 0;
  wapdu.asdu107.tsc = 0;
  wapdu.asdu107.time.year = agora->tm_year % 100;
  wapdu.asdu107.time.month = static_cast<uint8_t>(agora->tm_mon);
  wapdu.asdu107.time.mday = static_cast<uint8_t>(agora->tm_mday);
  wapdu.asdu107.time.hour = static_cast<uint8_t>(agora->tm_hour);
  wapdu.asdu107.time.min = static_cast<uint8_t>(agora->tm_min);
  wapdu.asdu107.time.msec = static_cast<uint16_t>(agora->tm_sec * 1000);
  wapdu.asdu107.time.su = static_cast<uint8_t>(agora->tm_isdst);
  wapdu.asdu107.time.iv = 0;
  wapdu.asdu107.time.wday = static_cast<uint8_t>(agora->tm_wday);
  wapdu.asdu107.time.res1 = 0;
  wapdu.asdu107.time.res2 = 0;
  wapdu.asdu107.time.res3 = 0;
  wapdu.asdu107.time.res4 = 0;

  sendTCP(reinterpret_cast<char*>(&wapdu), 22 + 2);
  VS += 2;

  mLog.pushMsg("     TEST COMMAND CONF ");
}

void iec104_class::sendStartDTACT() {
  // send STARTDTACT: enable data transfer
  iec_apdu apdu;
  apdu.start = START;
  apdu.length = 4;
  apdu.NS = STARTDTACT;
  apdu.NR = 0;
  sendTCP(reinterpret_cast<char*>(&apdu), 6);
  mLog.pushMsg("     STARTDTACT");
  tout_startdtact = t1_startdtact;
}

// tcp packet ready to be read from connection with the iec104 slave
void iec104_class::packetReadyTCP() {
  iec_apdu apdu;
  unsigned char* br;
  br = reinterpret_cast<unsigned char*>(&apdu);
  int bytesrec;
  unsigned char byt;
  int len;
  char buflog[10000];

  while (true) {

    if (!broken_msg) {
      // look for a START
      do {
        bytesrec = readTCP(reinterpret_cast<char*>(br), 1);
        if (bytesrec == 0)
          return;
        byt = br[0];
      } while (byt != START);
      bytesrec = readTCP(reinterpret_cast<char*>(br + 1), 1); // length of apdu
      if (bytesrec == 0)
        return;
    }

    len = br[1];
    if (len < 4) { // apdu length must be >= 4
      broken_msg = false;
      mLog.pushMsg("R--> ERROR: INVALID FRAME");
      continue;
    }

    waitBytes(len, 500);
    bytesrec = readTCP(reinterpret_cast<char*>(br + 2), len); // read the remaining of the apdu
    if (bytesrec == 0) {
      mLog.pushMsg("R--> Broken apdu");
      broken_msg = true;
      return;
    } else if (bytesrec < len) {
      int missing = len - bytesrec;
      sprintf(buflog, "R--> There should be more to read (%d of %d): ", missing, len);
      mLog.pushMsg(buflog);
      waitBytes(missing, 500);
      int bytesrec2 = readTCP(reinterpret_cast<char*>(br + 2 + bytesrec), missing); // read the remaining of the apdu
      sprintf(buflog, "R--> Readed more %d", bytesrec2);
      mLog.pushMsg(buflog);
      if (bytesrec2 != missing) {
        mLog.pushMsg("R--> Broken apdu!");
        broken_msg = true;
        return;
      }
    }

    //if ( apdu.asduh.ca != slaveAddress && apdu.asduh.ca != slaveASDUAddrCmd && len>4 )
    //  {
    //  broken_msg=false;
    //  mLog.pushMsg("R--> ASDU WITH UNEXPECTED ORIGIN! Ignoring...");
    //  // continue;
    //  }

    broken_msg = false;

    if (mLog.isLogging()) {
      sprintf(buflog, "R--> %03d: ", len + 2);
      int lim = 100;
      for (int i = 0; i < len + 2 && i < lim; i++) { // log up to 50 caracteres
        sprintf(buflog + strlen(buflog), "%02x ", br[i]);
      }
      if (len > lim - 2)
        sprintf(buflog + strlen(buflog), "...");
      mLog.pushMsg(buflog);
    }

    userprocAPDU(&apdu, len + 2);
    parseAPDU(&apdu, len + 2);
    if (bytesAvailableTCP() == 0)
      break;
  }
}


void iec104_class::LogFrame(char* frame, int size, bool is_send) {
  char buflog[10000];
  char* cp = frame;

  if (mLog.isLogging()) {
    if (is_send)
      sprintf(buflog, "T<-- %03d: ", int(size));
    else
      sprintf(buflog, "R--> %03d: ", int(size));
    int lim = 100;
    for (int i = 0; i < size && i < lim; i++) { // log up to 50 caracteres
      sprintf(buflog + strlen(buflog), "%02x ", static_cast<unsigned char>(cp[i]));
    }
    if (size > lim)
      sprintf(buflog + strlen(buflog), "...");
    mLog.pushMsg(buflog);
  }
}

char* iec104_class::trim(char* s) {
  int i;
  if (!s)
    return nullptr;   // handle NULL string
  if (!*s)
    return s;      // handle empty string
  while (isspace(*s))
    s++;   // skip left side white spaces
  for (i = int(strlen(s)) - 1; (isspace(s[i])); i--) ;    // skip right side white spaces
  s[i + 1] = '\0';
  return s;
}

// Log point, write to log when address is -1
void iec104_class::LogPoint(char* buf, int address, double val, char* qualifier, cp56time2a* timetag) {

  if (mLog.isLogging()) {
    if (address == -1) {
      mLog.pushMsg(buf);
      strcpy(buf, "     ");
      return;
    }

    if (ceil(val) == val)   // test val for integer whole value
      sprintf(buf + strlen(buf), "[%d %1.0f %s", address, val, qualifier);
    else
      sprintf(buf + strlen(buf), "[%d %1.3f %s", address, val, qualifier);

    trim(buf);

    if (timetag != nullptr)
      sprintf(buf + strlen(buf), " %04d/%02d/%02d %02d:%02d:%02d.%03d%s%s",
                timetag->year + 2000,
                timetag->month,
                timetag->mday,
                timetag->hour,
                timetag->min,
                timetag->msec / 1000,
                timetag->msec % 1000,
                timetag->iv ? ".iv" : "",
                timetag->su ? ".su" : "");
    sprintf(buf + strlen(buf), "] ");
  }
}

void iec104_class::parseAPDU(iec_apdu* papdu, int sz, bool accountandrespond) {
  iec_apdu wapdu;      // buffer to assemble apdu to send
  string qs, qsa;
  stringstream oss;
  unsigned short VR_NEW;

  if (papdu->start != START) {
    // invalid frame
    mLog.pushMsg("R--> ERROR: NO START IN FRAME");
    return;
  }

  if (sz == 6) {
    // Control messages
    if (accountandrespond)
      switch (papdu->NS) {
        case STARTDTACT:
          mLog.pushMsg("     STARTDTACT");
          wapdu.start = START;
          wapdu.length = 4;
          wapdu.NS = STARTDTCON;
          wapdu.NR = 0;
          sendTCP(reinterpret_cast<char*>(&wapdu), 6);
          mLog.pushMsg("     STARTDTCON");
          break;

        case TESTFRACT:
          mLog.pushMsg("     TESTFRACT");
          wapdu.start = START;
          wapdu.length = 4;
          wapdu.NS = TESTFRCON;
          wapdu.NR = 0;
          sendTCP(reinterpret_cast<char*>(&wapdu), 6);
          mLog.pushMsg("     TESTFRCON");
          break;

        case STARTDTCON:
          mLog.pushMsg("     STARTDTCON");
          tout_startdtact = -1; // flag confirmation of STARTDT, not to timeout
          TxOk = true;
          tout_gi = 15; // request GI when communication starts
          break;

        case STOPDTACT:
          mLog.pushMsg("     STOPDTACT");
          // only slave responds
          break;

        case STOPDTCON:
          mLog.pushMsg("     STOPDTCON");
          // do what?
          break;

        case TESTFRCON:
          mLog.pushMsg("     TESTFRCON");
          // do what?
          break;

        case SUPERVISORY:
          mLog.pushMsg("     SUPERVISORY");
          // do what?
          break;

        default: // error
          mLog.pushMsg("     ERROR: UNKNOWN CONTROL MESSAGE");
          break;
      }

  } else {
    // data message

    if (accountandrespond) {
      VR_NEW = (papdu->NS & 0xFFFE);

      // if (VR_NEW != VR) {
      if (VR_NEW != VR && VR_NEW != 2) { //  Allow initial I-frame from Slave with TX=1
        // sequence error, must close and reopen connection
        mLog.pushMsg("*** SEQUENCE ERROR! **************************");
        if (seq_order_check) {
          disconnectTCP();
          return;
        }
      }

      VR = VR_NEW + 2;
    }

    oss.str("");
    oss << "     OA "
        << unsigned(papdu->asduh.oa)
        << " CA "
        << unsigned(papdu->asduh.ca)
        << " TI TYPE "
        << unsigned(papdu->asduh.type) << ":" << asduTiStr(papdu->asduh.type)
        << " CAUSE "
        << int(papdu->asduh.cause) << ":" << causeStr(papdu->asduh.cause)
        << " SQ "
        << unsigned(papdu->asduh.sq)
        << " ITEMS "
        << unsigned(papdu->asduh.num)
        << (papdu->asduh.pn == POSITIVE ? " POSITIVE" : " NEGATIVE")
        << (papdu->asduh.t ? " TEST" : "");


    mLog.pushMsg(oss.str().c_str());

    switch (papdu->asduh.type) {
      case M_SP_NA_1: { // 1: DIGITAL SINGLE
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type1* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq1.obj[i];
            if (i == 0)
              addr24 = papdu->sq1.ioa16 + (unsigned(papdu->sq1.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq1[i].obj;
            addr24 = papdu->nsq1[i].ioa16 + (unsigned(papdu->nsq1[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->sp;
          piecarr[i].sp = pobj->sp;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%s", pobj->sp ? "on " : "off ", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, nullptr);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_DP_NA_1: { // 3: DIGITAL DOUBLE
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type3* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq3.obj[i];
            if (i == 0)
              addr24 = papdu->sq3.ioa16 + (unsigned(papdu->sq3.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq3[i].obj;
            addr24 = papdu->nsq3[i].ioa16 + (unsigned(papdu->nsq3[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->dp;
          piecarr[i].dp = pobj->dp;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          if (mLog.isLogging()) {
            char buf[100];
            static const char* dblmsg[] = { "tra ", "off ", "on ", "ind " };
            sprintf(buf, "%s%s%s%s%s", dblmsg[pobj->dp], pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, nullptr);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_ST_NA_1: { // 5: step position
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type5* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq5.obj[i];
            if (i == 0)
              addr24 = papdu->sq5.ioa16 + (unsigned(papdu->sq5.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq5[i].obj;
            addr24 = papdu->nsq5[i].ioa16 + (unsigned(papdu->nsq5[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->mv;
          piecarr[i].t = pobj->t;
          piecarr[i].ov = pobj->ov;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%s%s", pobj->t ? "t " : "", pobj->ov ? "ov " : "", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, nullptr);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_ME_NA_1: { // 9: ANALOGIC NORMALIZED
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type9* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq9.obj[i];
            if (i == 0)
              addr24 = papdu->sq9.ioa16 + (unsigned(papdu->sq9.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq9[i].obj;
            addr24 = papdu->nsq9[i].ioa16 + (unsigned(papdu->nsq9[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->mv;
          piecarr[i].ov = pobj->ov;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%s", pobj->ov ? "ov " : "", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, nullptr);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_ME_ND_1: { // 21: ANALOGIC NORMALIZED WITHOUT QUALITY
		char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type21* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq21.obj[i];
            if (i == 0)
              addr24 = papdu->sq21.ioa16 + (unsigned(papdu->sq21.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq21[i].obj;
            addr24 = papdu->nsq21[i].ioa16 + (unsigned(papdu->nsq21[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->mv;
          piecarr[i].ov = 0;
          piecarr[i].bl = 0;
          piecarr[i].nt = 0;
          piecarr[i].sb = 0;
          piecarr[i].iv = 0;
          if (mLog.isLogging()) {
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), nullptr, nullptr);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;      case M_ME_NB_1: { // 11: ANALOGIC CONVERTED
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type11* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq11.obj[i];
            if (i == 0)
              addr24 = papdu->sq11.ioa16 + (unsigned(papdu->sq11.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq11[i].obj;
            addr24 = papdu->nsq11[i].ioa16 + (unsigned(papdu->nsq11[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].value = pobj->mv;
          piecarr[i].ov = pobj->ov;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%s", pobj->ov ? "ov " : "", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, nullptr);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_ME_NC_1: { // 13: ANALOGIC FLOATING POINT
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type13* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq13.obj[i];
            if (i == 0)
              addr24 = papdu->sq13.ioa16 + (unsigned(papdu->sq13.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq13[i].obj;
            addr24 = papdu->nsq13[i].ioa16 + (unsigned(papdu->nsq13[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->mv;
          piecarr[i].ov = pobj->ov;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%s", pobj->ov ? "ov " : "", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, nullptr);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_SP_TB_1: { // 30:  DIGITAL SINGLE WITH LONG TIME TAG
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type30* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq30.obj[i];
            if (i == 0)
              addr24 = papdu->sq30.ioa16 + (unsigned(papdu->sq30.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq30[i].obj;
            addr24 = papdu->nsq30[i].ioa16 + (unsigned(papdu->nsq30[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->sp;
          piecarr[i].sp = pobj->sp;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          piecarr[i].timetag = pobj->time;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%s", pobj->sp ? "on " : "off ", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, &piecarr[i].timetag);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_DP_TB_1: { // 31: DIGITAL DOUBLE WITH LONG TIME TAG
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type31* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq31.obj[i];
            if (i == 0)
              addr24 = papdu->sq31.ioa16 + (unsigned(papdu->sq31.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq31[i].obj;
            addr24 = papdu->nsq31[i].ioa16 + (unsigned(papdu->nsq31[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->dp;
          piecarr[i].dp = pobj->dp;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          piecarr[i].timetag = pobj->time;
          if (mLog.isLogging()) {
            char buf[100];
            static const char* dblmsg[] = { "tra ", "off ", "on ", "ind " };
            sprintf(buf, "%s%s%s%s%s", dblmsg[pobj->dp], pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, &piecarr[i].timetag);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_ST_TB_1: { // 32: TAP WITH TIME TAG
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type32* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq32.obj[i];
            if (i == 0)
              addr24 = papdu->sq32.ioa16 + (unsigned(papdu->sq32.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq32[i].obj;
            addr24 = papdu->nsq32[i].ioa16 + (unsigned(papdu->nsq32[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->mv;
          piecarr[i].t = pobj->t;
          piecarr[i].ov = pobj->ov;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          piecarr[i].timetag = pobj->time;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%s%s", pobj->t ? "t " : "", pobj->ov ? "ov " : "", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, &piecarr[i].timetag);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_PS_NA_1: { // Packed single point information with status change detection
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type20* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq20.obj[i];
            if (i == 0)
              addr24 = papdu->sq20.ioa16 + (unsigned(papdu->sq20.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq20[i].obj;
            addr24 = papdu->nsq20[i].ioa16 + (unsigned(papdu->nsq20[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].stcd.st = pobj->stcd.st;
          piecarr[i].stcd.cd = pobj->stcd.cd;
          piecarr[i].value = double(pobj->stcd.st);
          piecarr[i].ov = pobj->ov;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          if (mLog.isLogging()) {
            char buf[1000];
            sprintf(buf, "%s%s%s%s%s ST %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d CH %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d [1-16]",
                    pobj->ov ? "ov " : "", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "",
                    pobj->stcd.st1,
                    pobj->stcd.st2,
                    pobj->stcd.st3,
                    pobj->stcd.st4,
                    pobj->stcd.st5,
                    pobj->stcd.st6,
                    pobj->stcd.st7,
                    pobj->stcd.st8,
                    pobj->stcd.st9,
                    pobj->stcd.st10,
                    pobj->stcd.st11,
                    pobj->stcd.st12,
                    pobj->stcd.st13,
                    pobj->stcd.st14,
                    pobj->stcd.st15,
                    pobj->stcd.st16,
                    pobj->stcd.cd1,
                    pobj->stcd.cd2,
                    pobj->stcd.cd3,
                    pobj->stcd.cd4,
                    pobj->stcd.cd5,
                    pobj->stcd.cd6,
                    pobj->stcd.cd7,
                    pobj->stcd.cd8,
                    pobj->stcd.cd9,
                    pobj->stcd.cd10,
                    pobj->stcd.cd11,
                    pobj->stcd.cd12,
                    pobj->stcd.cd13,
                    pobj->stcd.cd14,
                    pobj->stcd.cd15,
                    pobj->stcd.cd16
                   );
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].stcd.st), buf, nullptr);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_BO_NA_1: { // 7 bitstring
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type7* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq7.obj[i];
            if (i == 0)
              addr24 = papdu->sq7.ioa16 + (unsigned(papdu->sq7.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq7[i].obj;
            addr24 = papdu->nsq7[i].ioa16 + (unsigned(papdu->nsq7[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].bsi = pobj->bsi;
          piecarr[i].value = double(pobj->bsi.bsi);
          piecarr[i].ov = pobj->ov;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          if (mLog.isLogging()) {
            char buf[1000];
            sprintf(buf, "%s%s%s%s%s ST %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d% d%d%d%d %d%d%d%d [1-32]",
                    pobj->ov ? "ov " : "", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "",
                    pobj->bsi.st1,
                    pobj->bsi.st2,
                    pobj->bsi.st3,
                    pobj->bsi.st4,
                    pobj->bsi.st5,
                    pobj->bsi.st6,
                    pobj->bsi.st7,
                    pobj->bsi.st8,
                    pobj->bsi.st9,
                    pobj->bsi.st10,
                    pobj->bsi.st11,
                    pobj->bsi.st12,
                    pobj->bsi.st13,
                    pobj->bsi.st14,
                    pobj->bsi.st15,
                    pobj->bsi.st16,
                    pobj->bsi.st17,
                    pobj->bsi.st18,
                    pobj->bsi.st19,
                    pobj->bsi.st20,
                    pobj->bsi.st21,
                    pobj->bsi.st22,
                    pobj->bsi.st23,
                    pobj->bsi.st24,
                    pobj->bsi.st25,
                    pobj->bsi.st26,
                    pobj->bsi.st27,
                    pobj->bsi.st28,
                    pobj->bsi.st29,
                    pobj->bsi.st31,
                    pobj->bsi.st32
                   );
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].bsi.bsi), buf, nullptr);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_BO_TB_1: { // 33 bitstring with time tag
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type33* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq33.obj[i];
            if (i == 0)
              addr24 = papdu->sq33.ioa16 + (unsigned(papdu->sq33.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq33[i].obj;
            addr24 = papdu->nsq33[i].ioa16 + (unsigned(papdu->nsq33[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].bsi = pobj->bsi;
          piecarr[i].value = double(pobj->bsi.bsi);
          piecarr[i].ov = pobj->ov;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          piecarr[i].timetag = pobj->time;
          if (mLog.isLogging()) {
            char buf[1000];
            sprintf(buf, "%s%s%s%s%s ST %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d% d%d%d%d %d%d%d%d [1-32]",
                    pobj->ov ? "ov " : "", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "",
                    pobj->bsi.st1,
                    pobj->bsi.st2,
                    pobj->bsi.st3,
                    pobj->bsi.st4,
                    pobj->bsi.st5,
                    pobj->bsi.st6,
                    pobj->bsi.st7,
                    pobj->bsi.st8,
                    pobj->bsi.st9,
                    pobj->bsi.st10,
                    pobj->bsi.st11,
                    pobj->bsi.st12,
                    pobj->bsi.st13,
                    pobj->bsi.st14,
                    pobj->bsi.st15,
                    pobj->bsi.st16,
                    pobj->bsi.st17,
                    pobj->bsi.st18,
                    pobj->bsi.st19,
                    pobj->bsi.st20,
                    pobj->bsi.st21,
                    pobj->bsi.st22,
                    pobj->bsi.st23,
                    pobj->bsi.st24,
                    pobj->bsi.st25,
                    pobj->bsi.st26,
                    pobj->bsi.st27,
                    pobj->bsi.st28,
                    pobj->bsi.st29,
                    pobj->bsi.st31,
                    pobj->bsi.st32
                   );
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].bsi.bsi), buf, &piecarr[i].timetag);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_ME_TD_1: { //34 MEASURED VALUE, NORMALIZED WITH TIME TAG
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type34* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq34.obj[i];
            if (i == 0)
              addr24 = papdu->sq34.ioa16 + (unsigned(papdu->sq34.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq34[i].obj;
            addr24 = papdu->nsq34[i].ioa16 + (unsigned(papdu->nsq34[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->mv;
          piecarr[i].ov = pobj->ov;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          piecarr[i].timetag.mday = pobj->time.mday;
          piecarr[i].timetag.month = pobj->time.month;
          piecarr[i].timetag.year = pobj->time.year;
          piecarr[i].timetag.hour = pobj->time.hour;
          piecarr[i].timetag.min = pobj->time.min;
          piecarr[i].timetag.msec = pobj->time.msec;
          piecarr[i].timetag.iv = pobj->time.iv;
          piecarr[i].timetag.su = pobj->time.su;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%s", pobj->ov ? "ov " : "", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, &piecarr[i].timetag);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_ME_TE_1: { //35 MEASURED VALUE, SCALED WITH TIME TAG
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type35* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq35.obj[i];
            if (i == 0)
              addr24 = papdu->sq35.ioa16 + (unsigned(papdu->sq35.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq35[i].obj;
            addr24 = papdu->nsq35[i].ioa16 + (unsigned(papdu->nsq35[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->mv;
          piecarr[i].ov = pobj->ov;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          piecarr[i].timetag.mday = pobj->time.mday;
          piecarr[i].timetag.month = pobj->time.month;
          piecarr[i].timetag.year = pobj->time.year;
          piecarr[i].timetag.hour = pobj->time.hour;
          piecarr[i].timetag.min = pobj->time.min;
          piecarr[i].timetag.msec = pobj->time.msec;
          piecarr[i].timetag.iv = pobj->time.iv;
          piecarr[i].timetag.su = pobj->time.su;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%s", pobj->ov ? "ov " : "", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, &piecarr[i].timetag);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_ME_TF_1: { // 36 MEASURED VALUE, FLOATING POINT WITH TIME TAG
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type36* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq36.obj[i];
            if (i == 0)
              addr24 = papdu->sq36.ioa16 + (unsigned(papdu->sq36.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq36[i].obj;
            addr24 = papdu->nsq36[i].ioa16 + (unsigned(papdu->nsq36[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->mv;
          piecarr[i].ov = pobj->ov;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          piecarr[i].timetag.mday = pobj->time.mday;
          piecarr[i].timetag.month = pobj->time.month;
          piecarr[i].timetag.year = pobj->time.year;
          piecarr[i].timetag.hour = pobj->time.hour;
          piecarr[i].timetag.min = pobj->time.min;
          piecarr[i].timetag.msec = pobj->time.msec;
          piecarr[i].timetag.iv = pobj->time.iv;
          piecarr[i].timetag.su = pobj->time.su;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%s", pobj->ov ? "ov " : "", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "");
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, &piecarr[i].timetag);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_IT_NA_1: { // 15 = integrated totals without time tag
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type15* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq15.obj[i];
            if (i == 0)
              addr24 = papdu->sq15.ioa16 + (unsigned(papdu->sq15.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq15[i].obj;
            addr24 = papdu->nsq15[i].ioa16 + (unsigned(papdu->nsq15[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].bcr = pobj->bcr;
          piecarr[i].value = double(pobj->bcr);
          piecarr[i].cadj = pobj->ca;
          piecarr[i].cy = pobj->cy;
          piecarr[i].sq = pobj->sq;
          piecarr[i].iv = pobj->iv;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%u", pobj->ca ? "ca " : "", pobj->cy ? "cy " : "", pobj->iv ? "iv " : "", "sq=", pobj->sq);
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].bcr), buf, nullptr);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_IT_TB_1: { // 37 = integrated totals with time tag
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type37* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq37.obj[i];
            if (i == 0)
              addr24 = papdu->sq37.ioa16 + (unsigned(papdu->sq37.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq37[i].obj;
            addr24 = papdu->nsq37[i].ioa16 + (unsigned(papdu->nsq37[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].bcr = pobj->bcr;
          piecarr[i].value = double(pobj->bcr);
          piecarr[i].cadj = pobj->ca;
          piecarr[i].cy = pobj->cy;
          piecarr[i].sq = pobj->sq;
          piecarr[i].iv = pobj->iv;
          piecarr[i].timetag = pobj->time;
          if (mLog.isLogging()) {
            char buf[100];
            sprintf(buf, "%s%s%s%s%u", pobj->ca ? "ca " : "", pobj->cy ? "cy " : "", pobj->iv ? "iv " : "", "sq=", pobj->sq);
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].bcr), buf, &piecarr[i].timetag);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_EP_TD_1: { // 38 = Event of protection equipment with CP56Time2a time tag
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type38* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq38.obj[i];
            if (i == 0)
              addr24 = papdu->sq38.ioa16 + (unsigned(papdu->sq38.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq38[i].obj;
            addr24 = papdu->nsq38[i].ioa16 + (unsigned(papdu->nsq38[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->es;
          piecarr[i].dp = pobj->es;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          piecarr[i].elapsed_time = pobj->elapsed;
          piecarr[i].timetag.mday = pobj->time.mday;
          piecarr[i].timetag.month = pobj->time.month;
          piecarr[i].timetag.year = pobj->time.year;
          piecarr[i].timetag.hour = pobj->time.hour;
          piecarr[i].timetag.min = pobj->time.min;
          piecarr[i].timetag.msec = pobj->time.msec;
          piecarr[i].timetag.iv = pobj->time.iv;
          piecarr[i].timetag.su = pobj->time.su;
          if (mLog.isLogging()) {
            char buf[200];
            static const char* dblmsg[] = { "ind0 ", "off ", "on ", "ind3 " };
            sprintf(buf, "%s%s%s%s%s%s %dms", dblmsg[pobj->es], pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "", pobj->ei ? "ei " : "", pobj->elapsed.milliseconds);
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, &piecarr[i].timetag);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_EP_TE_1: { // 39 = Packed start events of protection equipment with CP56Time2a time tag
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type39* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj =  &papdu->sq39.obj[i];
            if (i == 0)
              addr24 = papdu->sq39.ioa16 + (unsigned(papdu->sq39.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq39[i].obj;
            addr24 = papdu->nsq39[i].ioa16 + (unsigned(papdu->nsq39[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->spe.gs;
          piecarr[i].spe = pobj->spe;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          piecarr[i].elapsed_time = pobj->elapsed;
          piecarr[i].timetag.mday = pobj->time.mday;
          piecarr[i].timetag.month = pobj->time.month;
          piecarr[i].timetag.year = pobj->time.year;
          piecarr[i].timetag.hour = pobj->time.hour;
          piecarr[i].timetag.min = pobj->time.min;
          piecarr[i].timetag.msec = pobj->time.msec;
          piecarr[i].timetag.iv = pobj->time.iv;
          piecarr[i].timetag.su = pobj->time.su;
          if (mLog.isLogging()) {
            char buf[200];
            sprintf(buf, "%s%s%s%s%s%s%s%s%s%s%s %dms", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "", pobj->ei ? "ei " : "",
                    pobj->spe.gs ? "gs " : "",
                    pobj->spe.sl1 ? "sl1 " : "",
                    pobj->spe.sl2 ? "sl2 " : "",
                    pobj->spe.sl3 ? "sl3 " : "",
                    pobj->spe.sie ? "sie " : "",
                    pobj->spe.srd ? "srd " : "",
                    pobj->elapsed.milliseconds);
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, &piecarr[i].timetag);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case M_EP_TF_1: { // 40 = Packed output circuit information of protection equipment with CP56Time2a time tag
        char logpointbuf[15000] = "";
        unsigned int addr24 = 0;
        iec_type40* pobj;
        iec_obj* piecarr = new iec_obj [papdu->asduh.num];
        if (papdu->asduh.cause >= 20 && papdu->asduh.cause <= 36)
          GIObjectCnt += papdu->asduh.num;

        for (int i = 0; i < papdu->asduh.num; i++) {
          if (papdu->asduh.sq) {
            pobj = &papdu->sq40.obj[i];
            if (i == 0)
              addr24 = papdu->sq40.ioa16 + (unsigned(papdu->sq40.ioa8) << 16);
            else
              addr24++;
          } else {
            pobj =  &papdu->nsq40[i].obj;
            addr24 = papdu->nsq40[i].ioa16 + (unsigned(papdu->nsq40[i].ioa8) << 16);
          }

          piecarr[i].address = addr24;
          piecarr[i].ca = papdu->asduh.ca;
          piecarr[i].cause = papdu->asduh.cause;
          piecarr[i].pn = papdu->asduh.pn;
          piecarr[i].test = papdu->asduh.t;
          piecarr[i].type = papdu->asduh.type;
          piecarr[i].value = pobj->oci.gc;
          piecarr[i].oci = pobj->oci;
          piecarr[i].bl = pobj->bl;
          piecarr[i].nt = pobj->nt;
          piecarr[i].sb = pobj->sb;
          piecarr[i].iv = pobj->iv;
          piecarr[i].elapsed_time = pobj->elapsed;
          piecarr[i].timetag.mday = pobj->time.mday;
          piecarr[i].timetag.month = pobj->time.month;
          piecarr[i].timetag.year = pobj->time.year;
          piecarr[i].timetag.hour = pobj->time.hour;
          piecarr[i].timetag.min = pobj->time.min;
          piecarr[i].timetag.msec = pobj->time.msec;
          piecarr[i].timetag.iv = pobj->time.iv;
          piecarr[i].timetag.su = pobj->time.su;
          if (mLog.isLogging()) {
            char buf[200];
            sprintf(buf, "%s%s%s%s%s%s%s%s%s %dms", pobj->bl ? "bl " : "", pobj->nt ? "nt " : "", pobj->sb ? "sb " : "", pobj->iv ? "iv " : "", pobj->ei ? "ei " : "",
                    pobj->oci.gc ? "gc " : "",
                    pobj->oci.cl1 ? "cl1 " : "",
                    pobj->oci.cl2 ? "cl2 " : "",
                    pobj->oci.cl3 ? "cl3 " : "",
                    pobj->elapsed.milliseconds);
            LogPoint(logpointbuf, int(piecarr[i].address), double(piecarr[i].value), buf, &piecarr[i].timetag);
          }
        }
        LogPoint(logpointbuf, -1, 0, nullptr, nullptr);
        dataIndication(piecarr, papdu->asduh.num);
        delete[] piecarr;
      }
      break;
      case C_SC_NA_1: { // SINGLE COMMAND
        iec_type45* pobj;
        pobj =  &papdu->nsq45.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "SINGLE COMMAND ADDRESS "
              << unsigned(papdu->nsq45.ioa16) + (unsigned(papdu->nsq45.ioa8) << 16)
              << " SCS "
              << unsigned(pobj->scs)
              << " QU "
              << int(pobj->qu)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq45.ioa16 + (unsigned(papdu->nsq45.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.scs = pobj->scs;
        iobj.qu = pobj->qu;
        iobj.se = pobj->se;
        commandActRespIndication(&iobj);
      }
      break;
      case C_DC_NA_1: { // DOUBLE COMMAND
        iec_type46* pobj;
        pobj =  &papdu->nsq46.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "DOUBLE COMMAND ADDRESS "
              << unsigned(papdu->nsq46.ioa16) + (unsigned(papdu->nsq46.ioa8) << 16)
              << " DCS "
              << unsigned(pobj->dcs)
              << " QU "
              << int(pobj->qu)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq46.ioa16 + (unsigned(papdu->nsq46.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.dcs = pobj->dcs;
        iobj.qu = pobj->qu;
        iobj.se = pobj->se;
        commandActRespIndication(&iobj);
      }
      break;
      case C_RC_NA_1: { // REG.STEP COMMAND
        iec_type47* pobj;
        pobj =  &papdu->nsq47.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "STEP REG. COMMAND ADDRESS "
              << unsigned(papdu->nsq47.ioa16) + (unsigned(papdu->nsq47.ioa8) << 16)
              << " RCS "
              << unsigned(pobj->rcs)
              << " QU "
              << int(pobj->qu)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq47.ioa16 + (unsigned(papdu->nsq47.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.rcs = pobj->rcs;
        iobj.qu = pobj->qu;
        iobj.se = pobj->se;
        commandActRespIndication(&iobj);
      }
      break;
      case C_SC_TA_1: { // SINGLE COMMAND WITH TIME
        iec_type58* pobj;
        pobj =  &papdu->nsq58.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "SINGLE COMMAND ADDRESS "
              << unsigned(papdu->nsq58.ioa16) + (unsigned(papdu->nsq58.ioa8) << 16)
              << " SCS "
              << unsigned(pobj->scs)
              << " QU "
              << int(pobj->qu)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq58.ioa16 + (unsigned(papdu->nsq58.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.scs = pobj->scs;
        iobj.qu = pobj->qu;
        iobj.se = pobj->se;
        commandActRespIndication(&iobj);
      }
      break;
      case C_DC_TA_1: { // DOUBLE COMMAND WITH TIME
        iec_type59* pobj;
        pobj =  &papdu->nsq59.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "DOUBLE COMMAND ADDRESS "
              << unsigned(papdu->nsq59.ioa16) + (unsigned(papdu->nsq59.ioa8) << 16)
              << " DCS "
              << unsigned(pobj->dcs)
              << " QU "
              << int(pobj->qu)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq59.ioa16 + (unsigned(papdu->nsq59.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.dcs = pobj->dcs;
        iobj.qu = pobj->qu;
        iobj.se = pobj->se;
        commandActRespIndication(&iobj);
      }
      break;
      case C_RC_TA_1: { // REG. STEP COMMAND WITH TIME
        iec_type60* pobj;
        pobj =  &papdu->nsq60.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "STEP REG. COMMAND ADDRESS "
              << unsigned(papdu->nsq60.ioa16) + (unsigned(papdu->nsq60.ioa8) << 16)
              << " RCS "
              << unsigned(pobj->rcs)
              << " QU "
              << int(pobj->qu)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq60.ioa16 + (unsigned(papdu->nsq60.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.rcs = pobj->rcs;
        iobj.qu = pobj->qu;
        iobj.se = pobj->se;
        commandActRespIndication(&iobj);
      }
      break;
      case C_SE_NA_1: { // NORMALISED COMMAND
        iec_type48* pobj;
        pobj =  &papdu->nsq48.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "NORMALISED COMMAND ADDRESS "
              << unsigned(papdu->nsq48.ioa16) + (unsigned(papdu->nsq48.ioa8) << 16)
              << " VAL "
              << pobj->nva
              << " QL "
              << int(pobj->ql)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq48.ioa16 + (unsigned(papdu->nsq48.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.qu = 0;
        iobj.se = pobj->se;
        iobj.value = pobj->nva;
        commandActRespIndication(&iobj);
      }
      break;
      case C_SE_TA_1: { // NORMALISED COMMAND WITH TIME
        iec_type61* pobj;
        pobj =  &papdu->nsq61.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "NORMALISED COMMAND ADDRESS "
              << unsigned(papdu->nsq61.ioa16) + (unsigned(papdu->nsq61.ioa8) << 16)
              << " VAL "
              << pobj->nva
              << " QL "
              << int(pobj->ql)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq61.ioa16 + (unsigned(papdu->nsq61.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.qu = 0;
        iobj.se = pobj->se;
        iobj.value = pobj->nva;
        commandActRespIndication(&iobj);
      }
      break;
      case C_SE_NB_1: { // SCALED COMMAND
        iec_type49* pobj;
        pobj =  &papdu->nsq49.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "SCALED COMMAND ADDRESS "
              << unsigned(papdu->nsq49.ioa16) + (unsigned(papdu->nsq49.ioa8) << 16)
              << " VAL "
              << pobj->sva
              << " QL "
              << int(pobj->ql)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq49.ioa16 + (unsigned(papdu->nsq49.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.qu = 0;
        iobj.se = pobj->se;
        iobj.value = pobj->sva;
        commandActRespIndication(&iobj);
      }
      break;
      case C_SE_TB_1: { // SCALED COMMAND WITH TIME
        iec_type62* pobj;
        pobj =  &papdu->nsq62.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "SCALED COMMAND ADDRESS "
              << unsigned(papdu->nsq62.ioa16) + (unsigned(papdu->nsq62.ioa8) << 16)
              << " VAL "
              << pobj->sva
              << " QL "
              << int(pobj->ql)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq62.ioa16 + (unsigned(papdu->nsq62.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.qu = 0;
        iobj.se = pobj->se;
        iobj.value = pobj->sva;
        iobj.timetag = pobj->time;
        commandActRespIndication(&iobj);
      }
      break;
      case C_SE_NC_1: { // FLOAT COMMAND
        iec_type50* pobj;
        pobj =  &papdu->nsq50.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "FLOAT COMMAND ADDRESS "
              << unsigned(papdu->nsq50.ioa16) + (unsigned(papdu->nsq50.ioa8) << 16)
              << " VAL "
              << pobj->r32
              << " QL "
              << int(pobj->ql)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq50.ioa16 + (unsigned(papdu->nsq50.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.qu = 0;
        iobj.se = pobj->se;
        iobj.value = pobj->r32;
        commandActRespIndication(&iobj);
      }
      break;
      case C_SE_TC_1: { // FLOAT COMMAND WITH TIME
        iec_type63* pobj;
        pobj = &papdu->nsq63.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "FLOAT COMMAND ADDRESS "
              << unsigned(papdu->nsq63.ioa16) + (unsigned(papdu->nsq63.ioa8) << 16)
              << " VAL "
              << pobj->r32
              << " QL "
              << int(pobj->ql)
              << " SE "
              << unsigned(pobj->se);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq63.ioa16 + (unsigned(papdu->nsq63.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.qu = 0;
        iobj.se = pobj->se;
        iobj.value = pobj->r32;
        commandActRespIndication(&iobj);
      }
      break;
      case M_EI_NA_1: //70
        mLog.pushMsg("R--> END OF INITIALIZATION");
        break;
      case INTERROGATION: // GI
        tout_gi = gi_period; // restart count to next GI
        if (papdu->asduh.cause == ACTCONFIRM) {
          GIObjectCnt = 0;
          mLog.pushMsg("     INTERROGATION ACT CON ------------------------------------------------------------------------");
          interrogationActConfIndication();
        } else if (papdu->asduh.cause == ACTTERM) {
          mLog.pushMsg("     INTERROGATION ACT TERM ------------------------------------------------------------------------");
          oss.str("");
          oss << "     Total objects in Interrogation: "
              << GIObjectCnt;
          mLog.pushMsg(oss.str().c_str());

          interrogationActTermIndication();
        } else
          mLog.pushMsg("     INTERROGATION");
        break;
      case C_TS_TA_1: { // 107
        iec_type107* pobj;
        pobj = (iec_type107*)papdu->dados;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     TEST COMMAND COM TAG "
              << " TSC " << unsigned(pobj->tsc)
              << unsigned(pobj->time.year) << "year " << unsigned(pobj->time.month) << "month " << unsigned(pobj->time.mday) << "day "
              << unsigned(pobj->time.hour) << "hour " << unsigned(pobj->time.min) << "min " << unsigned(pobj->time.msec / 1000) << "sec "
              << unsigned(pobj->time.msec % 1000) << "msec";
          mLog.pushMsg(oss.str().c_str());
        }

        if (papdu->asduh.cause == ACTIVATION) {
          confTestCommand();
        }
      }
      break;
      case C_RD_NA_1: { // READ COMMAND
        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "READ COMMAND ADDRESS "
              << unsigned(papdu->asdu102.ioa16) + (unsigned(papdu->asdu102.ioa8) << 16);

          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->asdu102.ioa16 + (unsigned(papdu->asdu102.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        commandActRespIndication(&iobj);
      }
      break;
      case C_CI_NA_1: { // 101
        iec_type101* pobj;
        pobj = (iec_type101*)&papdu->asdu101;
        if (mLog.isLogging()) {
          oss.str("");
          oss << "     COUNTER INTERROGATION COMMAND, ADDRESS "
              << (unsigned(papdu->asdu101.ioa16) + (unsigned(papdu->asdu101.ioa8) << 16))
              << " FRZ "
              << pobj->frz
              << " RQT "
              << int(pobj->rqt);
          mLog.pushMsg(oss.str().c_str());
        }
      }
      break;
      case C_CS_NA_1: { // 103
        iec_type103* pobj;
        pobj = (iec_type103*)&papdu->asdu103;
        if (mLog.isLogging()) {
          oss.str("");
          oss << "     CLOCK SYNC COMMAND "
              << unsigned(pobj->time.year) << "year " << unsigned(pobj->time.month) << "month " << unsigned(pobj->time.mday) << "day "
              << unsigned(pobj->time.hour) << "hour " << unsigned(pobj->time.min) << "min " << unsigned(pobj->time.msec / 1000) << "sec "
              << unsigned(pobj->time.msec % 1000) << "msec";
          mLog.pushMsg(oss.str().c_str());
        }
      }
      break;
      case P_ME_NA_1: { // Parameter of measured value, normalized value
        iec_type110* pobj;
        pobj = &papdu->nsq110.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "PARAMETER OF MEASURED NORMALIZED VALUE, ADDRESS "
              << unsigned(papdu->nsq110.ioa16) + (unsigned(papdu->nsq110.ioa8) << 16)
              << " VAL "
              << pobj->nva
              << " KPA "
              << int(pobj->kpa)
              << " LPC "
              << unsigned(pobj->lpc)
              << " POP "
              << unsigned(pobj->pop);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq110.ioa16 + (unsigned(papdu->nsq110.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.se = 0;
        iobj.qu = pobj->kpa;
        iobj.kpa = pobj->kpa;
        iobj.pop = pobj->pop;
        iobj.lpc = pobj->lpc;
        iobj.value = pobj->nva;
        commandActRespIndication(&iobj);
      }
      break;
      case P_ME_NB_1: { // Parameter of scaled value, normalized value
        iec_type111* pobj;
        pobj =  &papdu->nsq111.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "PARAMETER OF MEASURED SCALED VALUE, ADDRESS "
              << unsigned(papdu->nsq111.ioa16) + (unsigned(papdu->nsq111.ioa8) << 16)
              << " VAL "
              << pobj->sva
              << " KPA "
              << int(pobj->kpa)
              << " LPC "
              << unsigned(pobj->lpc)
              << " POP "
              << unsigned(pobj->pop);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq111.ioa16 + (unsigned(papdu->nsq111.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.se = 0;
        iobj.qu = pobj->kpa;
        iobj.kpa = pobj->kpa;
        iobj.pop = pobj->pop;
        iobj.lpc = pobj->lpc;
        iobj.value = pobj->sva;
        commandActRespIndication(&iobj);
      }
      break;
      case P_ME_NC_1: { // Parameter of measured value, float value
        iec_type112* pobj;
        pobj =  &papdu->nsq112.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "PARAMETER OF FLOAT NORMALIZED VALUE, ADDRESS "
              << unsigned(papdu->nsq112.ioa16) + (unsigned(papdu->nsq112.ioa8) << 16)
              << " VAL "
              << pobj->r32
              << " KPA "
              << int(pobj->kpa)
              << " LPC "
              << unsigned(pobj->lpc)
              << " POP "
              << unsigned(pobj->pop);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq110.ioa16 + (unsigned(papdu->nsq110.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.se = 0;
        iobj.qu = pobj->kpa;
        iobj.kpa = pobj->kpa;
        iobj.pop = pobj->pop;
        iobj.lpc = pobj->lpc;
        iobj.value = pobj->r32;
        commandActRespIndication(&iobj);
      }
      break;
      case P_AC_NA_1: { // Parameter activation
        iec_type113* pobj;
        pobj =  &papdu->nsq113.obj;

        if (mLog.isLogging()) {
          oss.str("");
          oss << "     ";
          if (papdu->asduh.cause == ACTCONFIRM)
            oss << "ACTIVATION CONFIRMATION ";
          else if (papdu->asduh.cause == ACTTERM)
            oss << "ACTIVATION TERMINATION ";
          if (papdu->asduh.pn == POSITIVE)
            oss << "POSITIVE ";
          else
            oss << "NEGATIVE ";
          oss << "PARAMETER ACTIVATION, ADDRESS "
              << unsigned(papdu->nsq113.ioa16) + (unsigned(papdu->nsq113.ioa8) << 16)
              << " QPA "
              << unsigned(pobj->qpa);
          mLog.pushMsg(oss.str().c_str());
        }

        // send indication to user
        iec_obj iobj;
        iobj.address = papdu->nsq113.ioa16 + (unsigned(papdu->nsq113.ioa8) << 16);
        iobj.ca = papdu->asduh.ca;
        iobj.cause = papdu->asduh.cause;
        iobj.pn = papdu->asduh.pn;
        iobj.test = papdu->asduh.t;
        iobj.type = papdu->asduh.type;
        iobj.se = 0;
        iobj.qu = pobj->qpa;
        iobj.qpa = pobj->qpa;
        iobj.value = pobj->qpa;
        commandActRespIndication(&iobj);
      }
      break;
      default:
        mLog.pushMsg("!!! TYPE NOT IMPLEMENTED");
        break;
    }

    if (accountandrespond) {

      tout_testfr = t3_testfr;

      if (msg_supervisory) {
        // will wait t2 seconds or n messages to send supervisory window control
        if (tout_supervisory < 0)
          tout_supervisory = t2_supervisory;

        if (tout_supervisory > 0)
          tout_supervisory--;

        if (tout_supervisory == 0) {
          tout_supervisory = -1;
          sendSupervisory();
        }
      } else
        sendSupervisory();
    }
  }
}

void iec104_class::sendSupervisory() {
  stringstream oss;
  iec_apdu apdu;

  apdu.start = START;
  apdu.length = 4;
  apdu.NS = SUPERVISORY;
  apdu.NR = VR;
  sendTCP(reinterpret_cast<char*>(&apdu), 6);

  oss.str("");
  oss.setf(ios::hex, ios::basefield);
  oss << "     SUPERVISORY " << VR;
  mLog.pushMsg(oss.str().c_str());
}

bool iec104_class::sendCommand(iec_obj* obj) {
  iec_apdu apducmd;
  time_t tm1 = time(nullptr);
  tm* agora = localtime(&tm1);
  stringstream oss;

  obj->cause = ACTIVATION;

  if (obj->ca == 0)
    obj->ca = slaveAddress;

  switch (obj->type) {
    case C_SC_NA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq45);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq45.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq45.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq45.obj.scs = obj->scs;
      apducmd.nsq45.obj.res = 0;
      apducmd.nsq45.obj.qu = obj->qu;
      apducmd.nsq45.obj.se = obj->se;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      if (mLog.isLogging()) {
        oss.str("");
        oss << "     SINGLE COMMAND ADDRESS "
            << unsigned(obj->address)
            << " SCS "
            << unsigned(obj->scs)
            << " CA "
            << obj->ca
            << " QU "
            << unsigned(obj->qu)
            << " SE "
            << unsigned(obj->se);
        mLog.pushMsg(oss.str().c_str());
      }

      break;
    case C_DC_NA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq46);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq46.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq46.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq46.obj.dcs = obj->dcs;
      apducmd.nsq46.obj.qu = obj->qu;
      apducmd.nsq46.obj.se = obj->se;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     DOUBLE COMMAND ADDRESS "
          << unsigned(obj->address)
          << " DCS "
          << unsigned(obj->dcs)
          << " CA "
          << obj->ca
          << " QU "
          << unsigned(obj->qu)
          << " SE "
          << unsigned(obj->se);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_RC_NA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq47);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq47.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq47.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq47.obj.rcs = obj->rcs;
      apducmd.nsq47.obj.qu = obj->qu;
      apducmd.nsq47.obj.se = obj->se;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;
      oss.str("");
      oss << "     STEP REG. COMMAND ADDRESS "
          << unsigned(obj->address)
          << " RCS "
          << unsigned(obj->rcs)
          << " CA "
          << obj->ca
          << " QU "
          << unsigned(obj->qu)
          << " SE "
          << unsigned(obj->se);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_SC_TA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq58);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq58.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq58.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq58.obj.scs = obj->scs;
      apducmd.nsq58.obj.res = 0;
      apducmd.nsq58.obj.qu = obj->qu;
      apducmd.nsq58.obj.se = obj->se;
      apducmd.nsq58.obj.time.year = static_cast<uint8_t>(agora->tm_year % 100);
      apducmd.nsq58.obj.time.month = static_cast<uint8_t>(agora->tm_mon + 1);
      apducmd.nsq58.obj.time.mday = static_cast<uint8_t>(agora->tm_mday);
      apducmd.nsq58.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq58.obj.time.min = static_cast<uint8_t>(agora->tm_min);
      apducmd.nsq58.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq58.obj.time.msec = static_cast<uint16_t>(agora->tm_sec * 1000);
      apducmd.nsq58.obj.time.iv = 0;
      apducmd.nsq58.obj.time.su = static_cast<uint8_t>(agora->tm_isdst);
      apducmd.nsq58.obj.time.wday = static_cast<uint8_t>(agora->tm_wday);
      apducmd.nsq58.obj.time.res1 = 0;
      apducmd.nsq58.obj.time.res2 = 0;
      apducmd.nsq58.obj.time.res3 = 0;
      apducmd.nsq58.obj.time.res4 = 0;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     SINGLE COMMAND W/TIME ADDRESS "
          << unsigned(obj->address)
          << " SCS "
          << unsigned(obj->scs)
          << " CA "
          << obj->ca
          << " QU "
          << unsigned(obj->qu)
          << " SE "
          << unsigned(obj->se);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_DC_TA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq59);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq59.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq59.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq59.obj.dcs = obj->dcs;
      apducmd.nsq59.obj.qu = obj->qu;
      apducmd.nsq59.obj.se = obj->se;
      apducmd.nsq59.obj.time.year = static_cast<uint8_t>(agora->tm_year % 100);
      apducmd.nsq59.obj.time.month = static_cast<uint8_t>(agora->tm_mon + 1);
      apducmd.nsq59.obj.time.mday = static_cast<uint8_t>(agora->tm_mday);
      apducmd.nsq59.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq59.obj.time.min = static_cast<uint8_t>(agora->tm_min);
      apducmd.nsq59.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq59.obj.time.msec = static_cast<uint16_t>(agora->tm_sec * 1000);
      apducmd.nsq59.obj.time.iv = 0;
      apducmd.nsq59.obj.time.su = static_cast<uint8_t>(agora->tm_isdst);
      apducmd.nsq59.obj.time.wday = static_cast<uint8_t>(agora->tm_wday);
      apducmd.nsq59.obj.time.res1 = 0;
      apducmd.nsq59.obj.time.res2 = 0;
      apducmd.nsq59.obj.time.res3 = 0;
      apducmd.nsq59.obj.time.res4 = 0;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     DOUBLE COMMAND W/TIME ADDRESS "
          << unsigned(obj->address)
          << " DCS "
          << unsigned(obj->dcs)
          << " CA "
          << obj->ca
          << " QU "
          << unsigned(obj->qu)
          << " SE "
          << unsigned(obj->se);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_RC_TA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq60);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq60.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq60.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq60.obj.rcs = obj->rcs;
      apducmd.nsq60.obj.qu = obj->qu;
      apducmd.nsq60.obj.se = obj->se;
      apducmd.nsq60.obj.time.year = static_cast<uint8_t>(agora->tm_year % 100);
      apducmd.nsq60.obj.time.month = static_cast<uint8_t>(agora->tm_mon + 1);
      apducmd.nsq60.obj.time.mday = static_cast<uint8_t>(agora->tm_mday);
      apducmd.nsq60.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq60.obj.time.min = static_cast<uint8_t>(agora->tm_min);
      apducmd.nsq60.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq60.obj.time.msec = static_cast<uint16_t>(agora->tm_sec * 1000);
      apducmd.nsq60.obj.time.iv = 0;
      apducmd.nsq60.obj.time.su = static_cast<uint8_t>(agora->tm_isdst);
      apducmd.nsq60.obj.time.wday = static_cast<uint8_t>(agora->tm_wday);
      apducmd.nsq60.obj.time.res1 = 0;
      apducmd.nsq60.obj.time.res2 = 0;
      apducmd.nsq60.obj.time.res3 = 0;
      apducmd.nsq60.obj.time.res4 = 0;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;
      oss.str("");
      oss << "     STEP REG. COMMAND W/TIME ADDRESS "
          << unsigned(obj->address)
          << " RCS "
          << unsigned(obj->rcs)
          << " CA "
          << obj->ca
          << " QU "
          << unsigned(obj->qu)
          << " SE "
          << unsigned(obj->se);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_SE_NA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq48);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq48.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq48.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq48.obj.nva = short(obj->value);
      apducmd.nsq48.obj.ql = 0;
      apducmd.nsq48.obj.se = obj->se;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     NORMALISED COMMAND ADDRESS "
          << unsigned(obj->address)
          << " VAL "
          << short(obj->value)
          << " CA "
          << obj->ca
          << " SE "
          << unsigned(obj->se);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_SE_TA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq61);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq61.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq61.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq61.obj.nva = short(obj->value);
      apducmd.nsq61.obj.ql = 0;
      apducmd.nsq61.obj.se = obj->se;
      apducmd.nsq61.obj.time.year = static_cast<uint8_t>(agora->tm_year % 100);
      apducmd.nsq61.obj.time.month = static_cast<uint8_t>(agora->tm_mon + 1);
      apducmd.nsq61.obj.time.mday = static_cast<uint8_t>(agora->tm_mday);
      apducmd.nsq61.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq61.obj.time.min = static_cast<uint8_t>(agora->tm_min);
      apducmd.nsq61.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq61.obj.time.msec = static_cast<uint16_t>(agora->tm_sec * 1000);
      apducmd.nsq61.obj.time.iv = 0;
      apducmd.nsq61.obj.time.su = static_cast<uint8_t>(agora->tm_isdst);
      apducmd.nsq61.obj.time.wday = static_cast<uint8_t>(agora->tm_wday);
      apducmd.nsq61.obj.time.res1 = 0;
      apducmd.nsq61.obj.time.res2 = 0;
      apducmd.nsq61.obj.time.res3 = 0;
      apducmd.nsq61.obj.time.res4 = 0;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     NORMALISED COMMAND W/TIME ADDRESS "
          << unsigned(obj->address)
          << " VAL "
          << short(obj->value)
          << " CA "
          << obj->ca
          << " SE "
          << unsigned(obj->se);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_SE_NB_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq49);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq49.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq49.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq49.obj.sva = short(obj->value);
      apducmd.nsq49.obj.ql = 0;
      apducmd.nsq49.obj.se = obj->se;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     SCALED COMMAND ADDRESS "
          << unsigned(obj->address)
          << " VAL "
          << short(obj->value)
          << " CA "
          << obj->ca
          << " SE "
          << unsigned(obj->se);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_SE_TB_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq62);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq62.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq62.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq62.obj.sva = short(obj->value);
      apducmd.nsq62.obj.ql = 0;
      apducmd.nsq62.obj.se = obj->se;
      apducmd.nsq62.obj.time.year = static_cast<uint8_t>(agora->tm_year % 100);
      apducmd.nsq62.obj.time.month = static_cast<uint8_t>(agora->tm_mon + 1);
      apducmd.nsq62.obj.time.mday = static_cast<uint8_t>(agora->tm_mday);
      apducmd.nsq62.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq62.obj.time.min = static_cast<uint8_t>(agora->tm_min);
      apducmd.nsq62.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq62.obj.time.msec = static_cast<uint16_t>(agora->tm_sec * 1000);
      apducmd.nsq62.obj.time.iv = 0;
      apducmd.nsq62.obj.time.su = static_cast<uint8_t>(agora->tm_isdst);
      apducmd.nsq62.obj.time.wday = static_cast<uint8_t>(agora->tm_wday);
      apducmd.nsq62.obj.time.res1 = 0;
      apducmd.nsq62.obj.time.res2 = 0;
      apducmd.nsq62.obj.time.res3 = 0;
      apducmd.nsq62.obj.time.res4 = 0;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     SCALED COMMAND W/TIME ADDRESS "
          << unsigned(obj->address)
          << " VAL "
          << short(obj->value)
          << " CA "
          << obj->ca
          << " SE "
          << unsigned(obj->se);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_SE_NC_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq50);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq50.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq50.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq50.obj.r32 = float(obj->value);
      apducmd.nsq50.obj.ql = 0;
      apducmd.nsq50.obj.se = obj->se;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     FLOAT COMMAND ADDRESS "
          << unsigned(obj->address)
          << " VAL "
          << obj->value
          << " CA "
          << obj->ca
          << " SE "
          << unsigned(obj->se);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_SE_TC_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq63);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq63.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq63.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq63.obj.r32 = float(obj->value);
      apducmd.nsq63.obj.ql = 0;
      apducmd.nsq63.obj.se = obj->se;
      apducmd.nsq63.obj.time.year = static_cast<uint8_t>(agora->tm_year % 100);
      apducmd.nsq63.obj.time.month = static_cast<uint8_t>(agora->tm_mon + 1);
      apducmd.nsq63.obj.time.mday = static_cast<uint8_t>(agora->tm_mday);
      apducmd.nsq63.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq63.obj.time.min = static_cast<uint8_t>(agora->tm_min);
      apducmd.nsq63.obj.time.hour = static_cast<uint8_t>(agora->tm_hour);
      apducmd.nsq63.obj.time.msec = static_cast<uint16_t>(agora->tm_sec * 1000);
      apducmd.nsq63.obj.time.iv = 0;
      apducmd.nsq63.obj.time.su = static_cast<uint8_t>(agora->tm_isdst);
      apducmd.nsq63.obj.time.wday = static_cast<uint8_t>(agora->tm_wday);
      apducmd.nsq63.obj.time.res1 = 0;
      apducmd.nsq63.obj.time.res2 = 0;
      apducmd.nsq63.obj.time.res3 = 0;
      apducmd.nsq63.obj.time.res4 = 0;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     SCALED COMMAND W/TIME ADDRESS "
          << unsigned(obj->address)
          << " VAL "
          << obj->value
          << " CA "
          << obj->ca
          << " SE "
          << unsigned(obj->se);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_CS_NA_1: // Clock Sync
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.asdu103);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.asdu103.ioa8 = 0;
      apducmd.asdu103.ioa16 = 0;
      apducmd.asdu103.time = obj->timetag;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     CLOCK SYNC COMMAND "
          << " CA "
          << obj->ca << " "
          << unsigned(obj->timetag.year) << "year " << unsigned(obj->timetag.month) << "month " << unsigned(obj->timetag.mday) << "day "
          << unsigned(obj->timetag.hour) << "hour " << unsigned(obj->timetag.min) << "min " << unsigned(obj->timetag.msec / 1000) << "sec "
          << unsigned(obj->timetag.msec % 1000) << "msec";
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_RP_NA_1: // reset process command
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.asdu107);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.asdu105.qrp = static_cast<unsigned char>(obj->value);
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     RESET PROCESS COMMAND"
          << " QRP "
          << unsigned(apducmd.asdu105.qrp);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_TS_TA_1: // test command with time tag
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.asdu107);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.asdu107.ioa8 = 0;
      apducmd.asdu107.ioa16 = 0;
      apducmd.asdu107.time = obj->timetag;
      apducmd.asdu107.tsc = test_command_count;
      test_command_count++;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     TEST COMMAND WITH TIME TAG"
          << " TSC "
          << apducmd.asdu107.tsc << " "
          << unsigned(obj->timetag.year) << "year " << unsigned(obj->timetag.month) << "month " << unsigned(obj->timetag.mday) << "day "
          << unsigned(obj->timetag.hour) << "hour " << unsigned(obj->timetag.min) << "min " << unsigned(obj->timetag.msec / 1000) << "sec "
          << unsigned(obj->timetag.msec % 1000) << "msec";
      mLog.pushMsg(oss.str().c_str());
      break;
    case P_ME_NA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq110);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq110.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq110.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq110.obj.nva = short(obj->value);
      apducmd.nsq110.obj.kpa = obj->kpa;
      apducmd.nsq110.obj.lpc = obj->lpc;
      apducmd.nsq110.obj.pop = obj->pop;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     PARAMETER OF MEASURED NORMALIZED VALUE, ADDRESS "
          << unsigned(obj->address)
          << " CA "
          << obj->ca
          << " VAL "
          << short(obj->value)
          << " KPA "
          << unsigned(obj->kpa)
          << " POP "
          << unsigned(obj->pop)
          << " LPC "
          << unsigned(obj->lpc);
      mLog.pushMsg(oss.str().c_str());
      break;
    case P_ME_NB_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq111);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq111.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq111.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq111.obj.sva = short(obj->value);
      apducmd.nsq111.obj.kpa = obj->kpa;
      apducmd.nsq111.obj.lpc = obj->lpc;
      apducmd.nsq111.obj.pop = obj->pop;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     PARAMETER OF MEASURED SCALED VALUE, ADDRESS "
          << unsigned(obj->address)
          << " CA "
          << obj->ca
          << " VAL "
          << short(obj->value)
          << " KPA "
          << unsigned(obj->kpa)
          << " POP "
          << unsigned(obj->pop)
          << " LPC "
          << unsigned(obj->lpc);
      mLog.pushMsg(oss.str().c_str());
      break;
    case P_ME_NC_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq112);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq112.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq112.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq112.obj.r32 = float(obj->value);
      apducmd.nsq112.obj.kpa = obj->kpa;
      apducmd.nsq112.obj.lpc = obj->lpc;
      apducmd.nsq112.obj.pop = obj->pop;
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     PARAMETER OF MEASURED FLOAT VALUE, ADDRESS "
          << unsigned(obj->address)
          << " CA "
          << obj->ca
          << " VAL "
          << double(obj->value)
          << " KPA "
          << unsigned(obj->kpa)
          << " POP "
          << unsigned(obj->pop)
          << " LPC "
          << unsigned(obj->lpc);
      mLog.pushMsg(oss.str().c_str());
      break;
    case P_AC_NA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.nsq113);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.nsq113.ioa16 = obj->address & 0x0000FFFF;
      apducmd.nsq113.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.nsq113.obj.qpa = short(obj->qpa);
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     PARAMETER ACTIVATION, ADDRESS "
          << unsigned(obj->address)
          << " QPA "
          << short(obj->qpa)
          << " CA "
          << obj->ca;
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_CI_NA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.asdu101);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.asdu101.ioa16 = obj->address & 0x0000FFFF;
      apducmd.asdu101.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      apducmd.asdu101.frz = obj->qu;
      apducmd.asdu101.rqt = uint8_t(obj->value);
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     COUNTER INTERROGATION COMMAND, ADDRESS "
          << unsigned(obj->address)
          << " FRZ " << unsigned(obj->frz)
          << " RQT " << unsigned(obj->value);
      mLog.pushMsg(oss.str().c_str());
      break;
    case C_RD_NA_1:
      apducmd.start = START;
      apducmd.length = sizeof(apducmd.NS) + sizeof(apducmd.NR) + sizeof(apducmd.asduh) + sizeof(apducmd.asdu102);
      apducmd.NS = VS;
      apducmd.NR = VR;
      apducmd.asduh.type = obj->type;
      apducmd.asduh.num = 1;
      apducmd.asduh.sq = 0;
      apducmd.asduh.cause = obj->cause;
      apducmd.asduh.t = 0;
      apducmd.asduh.pn = 0;
      apducmd.asduh.oa = masterAddress;
      apducmd.asduh.ca = obj->ca;
      apducmd.asdu102.ioa16 = obj->address & 0x0000FFFF;
      apducmd.asdu102.ioa8 = static_cast<uint8_t>(obj->address >> 16);
      sendTCP(reinterpret_cast<char*>(&apducmd), apducmd.length + sizeof(apducmd.start) + sizeof(apducmd.length));
      VS += 2;

      oss.str("");
      oss << "     READ COMMAND, ADDRESS "
          << unsigned(obj->address);
      mLog.pushMsg(oss.str().c_str());
      break;
    default:
      return false;
  }

  return true;
}

