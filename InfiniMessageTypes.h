#ifndef INFINI_MESSAGE_TYPES_H
#define INFINI_MESSAGE_TYPES_H

#include "InfiniCommon.h"
#include <cstdio>
#include <cstring>

namespace INFI {
  
  template <size_t BUFFER_SIZE, typename T = unsigned char>
  struct InfiniMessage  {
    InfiniMessage() :
      bufferSize(BUFFER_SIZE), 
      actualLen(0)
    {
      reset();
    }
    
    const size_t bufferSize;
    T val[BUFFER_SIZE];
    size_t actualLen;

    void reset() {
      memset(val, '\0', bufferSize);
      actualLen = 0;
    }
  };

  struct InfiniCommand : public InfiniMessage<MAX_CMD_SZ>
  {};

  struct InfiniResponse: public InfiniMessage<MAX_RESPONSE_SZ, char>
  {
    COMMAND_TYPE cmdType;
  };

  struct InfiniParseResult: public InfiniMessage<MAX_RESPONSE_SZ, char>
  {
    char error[256] = {'\0'};
    bool hasError = false;
  };
  
}

#endif
