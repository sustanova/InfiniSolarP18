#ifndef INFINI_RESPONSE_PARSER
#define INFINI_RESPONSE_PARSER

// #include <Arduino.h>
#include "InfiniMessageTypes.h"
#include "InfiniDataTypes.h"

namespace INFI {
  
  class InfiniResponseParser {
    public:
    char parsed[MAX_RESPONSE_SZ] = {'\r'};
    
    //! The publicly available ParseResult object.
    InfiniParseResult result;
  
    InfiniResponseParser();
    
    unsigned long fromILCurrentTimetoUnixTime(const char* in, size_t inSize);
    size_t fromILCurrentTimeToILCurrentDay(const char* in, size_t inSize);
    unsigned long fromInfiniGenEnergyToULong(const char* in, size_t inSize);
    size_t fromILGSToGeneralStatus(const char *in, size_t inSize);
    
    void parseQueryResponse(COMMAND_TYPE commandType, InfiniResponse &response);
    void parseUpdateResponse(COMMAND_TYPE commandType, InfiniResponse &response);
  
    private:
    bool checkStartTokenD(const char* in);
    bool checkDigits(const char* in, BYTE correctDigits);
    bool checkArraysEqual(const char* a1, const char* a2, BYTE startPos1, BYTE startPos2, BYTE numElems);
    bool checkQueryResponseBasic(const char* in, size_t inSize, BYTE commandSz);
  };
}
#endif
