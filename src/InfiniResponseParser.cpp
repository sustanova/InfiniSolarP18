#include "InfiniResponseParser.h"
#include <string.h>
#include <time.h>
#include "ArduinoJson.h"

namespace INFI {
  
  InfiniResponseParser::InfiniResponseParser() {
  }
  
  unsigned long InfiniResponseParser::fromILCurrentTimetoUnixTime(const char* in, size_t inSize) {
    if (!checkQueryResponseBasic(in, inSize, RESP_TO_END_SZS[CURRENT_TIME])) {
      return 0;
    }
  
    tm tms;
    const int S = START_OFFSET_SZ;
    tms.tm_year = (in[S+0] - '0') * 1000 + (in[S+1] - '0') * 100 + (in[S+2] - '0') * 10 + (in[S+3] - '0') - 1900; // Years since 1900
    tms.tm_mon = (in[S+4] - '0') * 10 + (in[S+5] - '0') - 1; // Months since Jan
    tms.tm_mday = (in[S+6] - '0') * 10 + (in[S+7] - '0');
    tms.tm_hour = (in[S+8] - '0') * 10 + (in[S+9] - '0');
    tms.tm_min = (in[S+10] - '0') * 10 + (in[S+11] - '0');
    tms.tm_sec = (in[S+12] - '0') * 10 + (in[S+13] - '0');
    tms.tm_isdst = 0;
    time_t t = mktime(&tms);
  
    sprintf(parsed, "%ld", t);
    
    return t;
  }
  
  size_t InfiniResponseParser::fromILCurrentTimeToILCurrentDay(const char* in, size_t inSize) {
    if (!checkQueryResponseBasic(in, inSize, RESP_TO_END_SZS[CURRENT_TIME])) {
      return 0;
    }
    
    const int S = START_OFFSET_SZ;
    memset(parsed, '\0', MAX_RESPONSE_SZ);
    for (BYTE i = 0; i < TIME_DAY_SZ; ++i) {
      parsed[i] = in[S+i];
    }
    return TIME_DAY_SZ;
  }
  
  unsigned long InfiniResponseParser::fromInfiniGenEnergyToULong(const char* in, size_t inSize) {
    if (!checkQueryResponseBasic(in, inSize, RESP_TO_END_SZS[GEN_ENERGY_DAY])) {
      return 0;
    }
    
    unsigned long energy;
    // 5 is START_TOKEN_SZ + DATA_LENGTH_SZ,
    // 8 is GEN_ENERGY_DAY_SZ - CRC_SZ - END_TOKEN_SZ
    sscanf(in,"%*5s%8lu%*s",&energy);
    return energy;
  }

  size_t InfiniResponseParser::fromILGSToGeneralStatus(const char *in, size_t inSize) {
    if (!checkQueryResponseBasic(in, inSize, RESP_TO_END_SZS[GENERAL_STATUS])) {
      return -1;
    }
    
    GeneralStatus gs;
    // Exact size is calculated with https://arduinojson.org/v6/assistant/ with a sample json.
    const uint16_t DOC_SZ = 812;
    StaticJsonDocument<DOC_SZ> doc; 
    
    // Read the data from in into gs.
    uint8_t settingsChanged;
    uint8_t loadConnection;
    // NOTE: Apparently sscanf is quite memory intensive. It might be better to just use indexes/loops to get the relevant data.
    sscanf(in, "%*5s%4f,%3f,%4f,%3f,%4d,%4d,%3hhd,%3f,%3f,%3f,%3d,%3d,%3hhd,%3hhd,%3hhd,%3hhd,%4hd,%4hd,%4f,%4f,%3hhd,%3hhd,%3hhd,%3hhd,%3hhd,%3hhd,%3hhd,%3hhd%*s",
           &(gs.gridVolt), &(gs.gridFreq), &(gs.acOutVolt), &(gs.acOutFreq), &(gs.acOutApparentPow), &(gs.acOutActivePow), 
           &(gs.outLoadPct), &(gs.battVolt), &(gs.battVoltSCC), &(gs.battVoltSCC2), &(gs.battDischargeCurr), &(gs.battChargeCurr), &(gs.battCapacity),
           &(gs.invHeatSinkTemp), &(gs.mppt1ChrgrTemp), &(gs.mppt2ChrgrTemp),
           &(gs.pv1InPow), &(gs.pv2InPow), &(gs.pv1InVolt), &(gs.pv2InVolt),
           &settingsChanged, &(gs.mppt1ChrgrStatus), &(gs.mppt2ChrgrStatus), &loadConnection, &(gs.battPowDir), &(gs.dcACPowDir), &(gs.linePowDir), &(gs.localParallelId));
    
    // Apply multipliers, other transforms
    gs.gridVolt *= 0.1;
    gs.gridFreq *= 0.1;
    gs.acOutVolt *= 0.1;
    gs.acOutFreq *= 0.1;
    gs.battVolt *= 0.1;
    gs.battVoltSCC *= 0.1;
    gs.battVoltSCC2 *= 0.1;
    gs.pv1InVolt *= 0.1;
    gs.pv2InVolt *= 0.1;
    gs.settingsChanged = (settingsChanged == 1) ? true : false;
    gs.loadConnection = (loadConnection == 1) ? true : false;
    
    // Store in doc
    doc["gridVolt"] = gs.gridVolt;
    doc["gridFreq"] = gs.gridFreq;
    doc["acOutVolt"] = gs.acOutVolt;
    doc["acOutFreq"] = gs.acOutFreq;
    doc["acOutApparentPow"] = gs.acOutApparentPow;
    doc["acOutActivePow"] = gs.acOutActivePow;
    doc["outLoadPct"] = gs.outLoadPct;
    doc["battVolt"] = gs.battVolt;
    doc["battVoltSCC"] = gs.battVoltSCC;
    doc["battVoltSCC2"] = gs.battVoltSCC2;
    doc["battDischargeCurr"] = gs.battDischargeCurr;
    doc["battChargeCurr"] = gs.battChargeCurr;
    doc["battCapacity"] = gs.battCapacity;
    doc["invHeatSinkTemp"] = gs.invHeatSinkTemp;
    doc["mppt1ChrgrTemp"] = gs.mppt1ChrgrTemp;
    doc["mppt2ChrgrTemp"] = gs.mppt2ChrgrTemp;
    doc["pv1InPow"] = gs.pv1InPow;
    doc["pv2InPow"] = gs.pv2InPow;
    doc["pv1InVolt"] = gs.pv1InVolt;
    doc["pv2InVolt"] = gs.pv2InVolt;
    doc["settingsChanged"] = settingsChanged;
    doc["mppt1ChrgrStatus"] = gs.mppt1ChrgrStatus;
    doc["mppt2ChrgrStatus"] = gs.mppt2ChrgrStatus;
    doc["loadConnection"] = gs.loadConnection;
    doc["battPowDir"] = gs.battPowDir;
    doc["dcACPowDir"] = gs.dcACPowDir;
    doc["linePowDir"] = gs.linePowDir;
    doc["localParallelId"] = gs.localParallelId;

    // Serialize to parsed and return the size.
    return serializeJson(doc, parsed);
  }
    
  void InfiniResponseParser::parseQueryResponse(COMMAND_TYPE commandType, InfiniResponse &response) {
    // The response start with ^Dxxx, where xxx is the response to <cr> length.
    if (response.actualLen != START_OFFSET_SZ + RESP_TO_END_SZS[commandType]) {
      result.hasError = true;
      strcpy(result.error, 
             "The response to be parsed did not have the expected length.");
      return;
    }
    if (!checkStartTokenD(response.val)) {
      result.hasError = true;
      strcpy(result.error, "The query response did not begin with ^D.");
      return;
    }
    if (!checkDigits(response.val, RESP_TO_END_SZS[commandType])) {
      result.hasError = true;
      strcpy(result.error, 
             "The query response does not have the expected length.");
      return;
    }
  }

  void InfiniResponseParser::parseUpdateResponse(COMMAND_TYPE commandType, InfiniResponse &response) {
    // In this case the response is simple ^1<CRC><cr> or ^0<CRC><cr>.
    if (response.actualLen != START_TOKEN_SZ + CRC_SZ + END_TOKEN_SZ) {
      result.hasError = true;
      strcpy(result.error,
             "The response to be parsed did not have the expected length.");
      return;
    }
  }
  
  //private
  bool InfiniResponseParser::checkStartTokenD(const char* in) {
    char startChars[START_TOKEN_SZ] = {'^', 'D'};
    return checkArraysEqual(in, startChars, 0, 0, START_TOKEN_SZ);
  }
  
  bool InfiniResponseParser::checkDigits(const char* in, BYTE correctDigits) {
    char digitsChar[3];
    getThreeDigits(correctDigits, digitsChar, 0);
    return checkArraysEqual(in, digitsChar, START_TOKEN_SZ, 0, 3);
  }
  
  bool InfiniResponseParser::checkArraysEqual(const char* a1, const char* a2, BYTE startPos1, BYTE startPos2, BYTE numElems) {
    for (BYTE i = 0; i < numElems; ++i) {
      if (a1[i + startPos1] != a2[i + startPos2]) {
        return false;
      }
    }
    return true;
  }

  bool InfiniResponseParser::checkQueryResponseBasic(const char* in, size_t inSize, BYTE commandSz) {
    if (inSize != getFullSizeFromCommandSize(commandSz, false) || !checkStartTokenD(in) || !checkDigits(in, commandSz)) {
      return false;
    }
    return true;
  }
}
