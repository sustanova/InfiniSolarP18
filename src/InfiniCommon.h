#ifndef INFINI_COMMON_H
#define INFINI_COMMON_H

namespace INFI {

  typedef unsigned char BYTE; //1byte
  typedef unsigned short WORD; //2bytes
  //typedef unsigned long DWORD; //4bytes, not needed
  
  enum ACTION_TYPE { READ, UPDATE };

  /*!
   * An enum for representing the various commands listed in the protocol file.
   * I thought it might be good to use an enum because
   * 1. It might bring consistency to the make/send command API (not sure how true/helpful this is tho), and
   * 2. It can help index into various related pieces of info such as expected command/response lengths.
   */
  enum COMMAND_TYPE {
    CURRENT_TIME = 0, 
    TOTAL_GEN_ENERGY,
    GEN_ENERGY_YEAR,
    GEN_ENERGY_MONTH,
    GEN_ENERGY_DAY,
    GENERAL_STATUS,
    QUERY_RATED_INFORMATION,
    FAULT_WARNING_STATUS,
    QUERY_ENABLE_DISABLE_STATUS,
    QUERY_DEFAULT_VALUE,
    QUERY_MAX_CHARGING_CURRENT,
    QUERY_MAX_AC_CHARGING_CURRENT,
    SET_ENABLE_DISABLE_STATUS,
    SET_MAX_CHARGING_CURRENT,
    SET_MAX_AC_CHARGING_CURRENT,
    AC_OUT_FREQ_50,
    AC_OUT_FREQ_60,
    SET_OUTPUT_SOURCE_PRIORITY,
    SET_CHARGING_SOURCE_PRIORITY,
    SET_SOLAR_POWER_PRIORITY,
    SET_BATTERY_TYPE,
    SET_DATE_TIME
  };

  /*!
   * Command sizes per protocol manual i.e. the int represented by the 3 digits after ^P or ^S.
   * The command sizes in the protocol manual include the 2 <CRC> and 1 <cr> chars.
   */
  const BYTE CMD_TO_END_SZS[] = {
    4,   // ^P004T<CRC><cr>
    5,   // ^P005ET<CRC><cr>
    9,   // ^P009EY2019<CRC><cr>
    11,  // ^P011EM201902<CRC><cr>
    13,  // ^P013ED20190216<CRC><cr>
    5,   // ^P005GS<CRC><cr>
    7,   // ^P007PIRI<CRC><cr>
    6,   // ^P006FWS<CRC><cr>
    7,   // ^P007FLAG<CRC><cr>
    5,   // ^P005DI<CRC><cr>
    9,   // ^P009MCHGCR<CRC><cr>
    10,  // ^P010MUCHGCR<CRC><cr>
    6,   // ^S006Pmn<CRC><cr>
    13,  // ^S013MCHGCm,nnn<CRC><cr>
    14,  // ^S014MUCHGCm,nnn<CRC><cr>
    6,   // ^S006F50<CRC><cr>
    6,   // ^S006F60<CRC><cr>
    7,   // ^S007POPm<CRC><cr>
    9,   // ^S009PCPm,n<CRC><cr>
    7,   // ^S007PSPm<CRC><cr>
    7,   // ^S007PBTm<CRC><cr>
    18   // ^S018DATyymmddhhffss<CRC><cr>
  };

  const BYTE RESP_TO_END_SZS[] = {
    17,  // T, ^D017YYYYMMDDHHFFSS<CRC><cr>
    11,  // ET, ^D011NNNNNNNN<CRC><cr>
    11,  // EY, ^D011NNNNNNNN<CRC><cr>
    11,  // EM, ^D011NNNNNNNN<CRC><cr>
    11,  // ED, ^D011NNNNNNNN<CRC><cr>
    106, // GS, ^D106AAAA,BBB,CCCC,DDD,EEEE,FFFF,GGG,HHH,III,JJJ,KKK,LLL,MMM,NNN,OOO,PPP,QQQQ,RRRR,SSSS,TTTT,U,V,W,X,Y,Z,a,b<CRC><cr>
    85,  // PIRI, ^D085AAAA,BBB,CCCC,DDD,EEE,FFFF,GGGG,HHH,III,JJJ,KKK,LLL,MMM,N,OO,PPP,Q,R,S,T,U,V,W,Z,a<CRC><cr>
    37,  // FWS, ^D037AA,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q<CRC><cr>,
    20,  // FLAG, ^D020A,B,C,D,E,F,G,H,I<CRC><cr>
    68,  // DI, ^D068AAAA,BBB,C,DDD,EEE,FFF,GGG,HHH,III,JJ,K,L,M,N,O,P,S,T,U,V,W,X,Y,Z<CRC><cr>
    58,  // MCHGCR, ^D058AAA,BBB,CCC,DDD,EEE,FFF,GGG,HHH,III,JJJ,KKK,LLL,MMM,NNN<CRC><cr>
    30,  // MUCHGCR, ^D030AAA,BBB,CCC,DDD,EEE,FFF,GGG<CRC><cr>NNN<CRC><cr>
    0,   // Pmn, ^1<CRC><cr> or ^0<CRC><cr>
    0,   // MCHGCm,nnn, ^1<CRC><cr> or ^0<CRC><cr>
    0,   // MUCHGCm,nnn, ^1<CRC><cr> or ^0<CRC><cr>
    0,   // F50, ^1<CRC><cr> or ^0<CRC><cr>
    0,   // F60, ^1<CRC><cr> or ^0<CRC><cr>
    0,   // POPm, ^1<CRC><cr> or ^0<CRC><cr>
    0,   // PCPm,n, ^1<CRC><cr> or ^0<CRC><cr>
    0,   // PSPm, ^1<CRC><cr> or ^0<CRC><cr>
    0,   // PBTm, ^1<CRC><cr> or ^0<CRC><cr>
    0,   // DATyymmddhhffss, ^1<CRC><cr> or ^0<CRC><cr>
  };

  // Messaging baud
  const long SERIAL_BAUD = 2400;
  
  // Message Format sizes in bytes
  const BYTE START_TOKEN_SZ = 2;
  const BYTE DATA_LENGTH_SZ = 3;
  const BYTE CRC_SZ = 2;
  const BYTE END_TOKEN_SZ = 1;

  // Message Response sizes in bytes
  const BYTE MAX_CMD_SZ = 32;
  const int MAX_RESPONSE_SZ = 1024;

  // Other common sizes in bytes
  const BYTE START_OFFSET_SZ = START_TOKEN_SZ + DATA_LENGTH_SZ;
  const BYTE TIME_YEAR_SZ = 4; //2016
  const BYTE TIME_MON_SZ = TIME_YEAR_SZ + 2; //201611
  const BYTE TIME_DAY_SZ = TIME_MON_SZ + 2; //20161103

  /*! Derive the length of the entire message to be sent from the command size.
   *  This appears in the msg itself as the 3 digits after the start token.
   */
  BYTE getFullSizeFromCommandSize(BYTE commandSz, bool skipEndToken = false);
  
  //! Converts num to a 3 digit ascii, stores in outArr at startPos.
  void getThreeDigits(const BYTE num, BYTE *outArr, BYTE startPos);

  //! Converts num to a 3 digit ascii, stores in outArr at startPos.
  void getThreeDigits(const BYTE num, char *outArr, BYTE startPos);

  /*! Configure the ESP32 time for India.
   * This needs to be called outside any class constructor, in/after setup().
   * See https://github.com/espressif/arduino-esp32/issues/831#issuecomment-343755427
   */
  void setupGMTTimeForIndia();
  
  //! Get current day.
  void getToday(char *out);
}

#endif
