#include "InfiniCommon.h"
#include <time.h>
#include <stdio.h>

namespace INFI {
  BYTE getFullSizeFromCommandSize(BYTE commandSz, bool skipEndToken) {
    BYTE val = START_TOKEN_SZ + DATA_LENGTH_SZ + commandSz;
    if (skipEndToken) {
      return val - END_TOKEN_SZ;
    }
    return val;
  }

  
  void getThreeDigits(const BYTE num, BYTE *outArr, BYTE startPos) {
    outArr[startPos] = num / 100;
    BYTE rem = num - (outArr[startPos] * 100);
    outArr[startPos + 1] = rem / 10;
    outArr[startPos + 2] = rem - (outArr[startPos + 1] * 10);
    
    // Add 0x30 to everything, because 0x30 is ASCII for '0'.
    outArr[startPos] += 0x30;
    outArr[startPos + 1] += 0x30;
    outArr[startPos + 2] += 0x30;
  }

  void getThreeDigits(const BYTE num, char *outArr, BYTE startPos) {
    outArr[startPos] = num / 100;
    char rem = num - (outArr[startPos] * 100);
    outArr[startPos + 1] = rem / 10;
    outArr[startPos + 2] = rem - (outArr[startPos + 1] * 10);
    
    // Add 0x30 to everything, because 0x30 is ASCII for '0'.
    outArr[startPos] += 0x30;
    outArr[startPos + 1] += 0x30;
    outArr[startPos + 2] += 0x30;
  }

  void setupGMTTimeForIndia() {
    // 19800 = 5.5 * 3600 cuz India is +5.5hrs GMT.
    // 0 for Daylight savings.
    // "pool.ntp.org" is a "public ntp time server for everyone". See https://www.ntppool.org/en/
    // configTime(19800, 0, "pool.ntp.org");
  }

  void getToday(char *out) {
    time_t rawtime;
    struct tm * ptm;
    time(&rawtime);
    ptm = localtime (&rawtime);
    sprintf(out, "%4d%02d%02d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
  }
}
