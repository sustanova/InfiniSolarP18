#include "InfiniCommandMaker.h"
#include "InfiniCRC.h"

namespace INFI {
  InfiniCommandMaker::InfiniCommandMaker() :
    m_cmdToEndSz(0)
  {}
  
  void InfiniCommandMaker::makeCommand(COMMAND_TYPE commandType, const char* params) {
    // Reset all to null
    command.reset();

    // Get the cmdToEnd size in the protocol manual based on the commandType.
    m_cmdToEndSz = CMD_TO_END_SZS[commandType];
    
    // Insert the start token, length and command chars
    makeStartLengthCommand(commandType, params);
  
    // Calculate and insert the crc
    WORD crc = INFI::calc_crc_half(command.val, command.actualLen);
    command.val[command.actualLen] = crc >> 8;
    command.val[command.actualLen + 1] = crc & 0xff;
    command.val[command.actualLen + 2] = '\r';
    command.actualLen += CRC_SZ + END_TOKEN_SZ;
  }
  
  void InfiniCommandMaker::makeStartLengthCommand(COMMAND_TYPE commandType, const char* params) {
    if (commandType == CURRENT_TIME) {
      insertStartLengthCommand(READ, "T", "");
    } else if (commandType == TOTAL_GEN_ENERGY) {
      insertStartLengthCommand(READ, "ET", "");
    } else if (commandType == GEN_ENERGY_YEAR) {
      insertStartLengthCommand(READ, "EY", params);
    } else if (commandType == GEN_ENERGY_MONTH) {
      insertStartLengthCommand(READ, "EM", params);
    } else if (commandType == GEN_ENERGY_DAY) {
      insertStartLengthCommand(READ, "ED", params);
    } else if (commandType == GENERAL_STATUS) {
      insertStartLengthCommand(READ, "GS", "");
    } else if (commandType == QUERY_RATED_INFORMATION) {
      insertStartLengthCommand(READ, "PIRI", "");
    } else if (commandType == FAULT_WARNING_STATUS) {
      insertStartLengthCommand(READ, "FWS", "");
    } else if (commandType == QUERY_ENABLE_DISABLE_STATUS) {
      insertStartLengthCommand(READ, "FLAG", "");
    } else if (commandType == QUERY_DEFAULT_VALUE) {
      insertStartLengthCommand(READ, "DI", "");
    } else if (commandType == QUERY_MAX_CHARGING_CURRENT) {
      insertStartLengthCommand(READ, "MCHGCR", "");
    } else if (commandType == QUERY_MAX_AC_CHARGING_CURRENT) {
      insertStartLengthCommand(READ, "MUCHGCR", "");
    } else if (commandType == SET_ENABLE_DISABLE_STATUS) {
      insertStartLengthCommand(UPDATE, "P", params);
    } else if (commandType == SET_MAX_CHARGING_CURRENT) {
      insertStartLengthCommand(UPDATE, "MCHGC", params);
    } else if (commandType == SET_MAX_AC_CHARGING_CURRENT) {
      insertStartLengthCommand(UPDATE, "MUCHGC", params);
    } else if (commandType == AC_OUT_FREQ_50) {
      insertStartLengthCommand(UPDATE, "F50", "");
    } else if (commandType == AC_OUT_FREQ_60) {
      insertStartLengthCommand(UPDATE, "F60", "");
    } else if (commandType == SET_OUTPUT_SOURCE_PRIORITY) {
      insertStartLengthCommand(UPDATE, "POP", params);
    } else if (commandType == SET_CHARGING_SOURCE_PRIORITY) {
      insertStartLengthCommand(UPDATE, "PCP", params);
    } else if (commandType == SET_SOLAR_POWER_PRIORITY) {
      insertStartLengthCommand(UPDATE, "PSP", params);
    } else if (commandType == SET_BATTERY_TYPE) {
      insertStartLengthCommand(UPDATE, "PBT", params);
    } else if (commandType == SET_DATE_TIME) {
      insertStartLengthCommand(UPDATE, "DAT", params);
    }
  }
  
  void InfiniCommandMaker::insertStartLengthCommand(ACTION_TYPE actionType,
                                                   const char* commandChars,
                                                   const char* params) {
    // Insert the '^P' or '^S'.
    insertStartAndType(actionType);
    
    // Update the actual length
    BYTE cmdSz = strlen(commandChars);
    BYTE paramsSz = strlen(params);
    BYTE cmdNParamSz = m_cmdToEndSz - CRC_SZ - END_TOKEN_SZ;
    // Trim paramsSz if needed
    if (cmdSz + paramsSz > cmdNParamSz) {
      paramsSz = cmdNParamSz - cmdSz;
    }
  
    // Insert the cmd length per the protocol
    insertLength(m_cmdToEndSz);
    // The above should always succeed, so update actualLen
    command.actualLen = START_OFFSET_SZ;

    // Insert the command + params
    BYTE startOffset = START_OFFSET_SZ;
    for (BYTE i = 0; i < cmdSz; ++i) {
      command.val[startOffset + i] = (BYTE)commandChars[i]; 
    }
    command.actualLen += cmdSz;
    startOffset += cmdSz;
    for (BYTE i = 0; i < paramsSz; ++i) {
      command.val[startOffset + i] = (BYTE)params[i];
    }
    command.actualLen += paramsSz;
  }
  
  void InfiniCommandMaker::insertStartAndType(ACTION_TYPE actionType) {
    command.val[0] = '^';
    if (actionType == ACTION_TYPE::UPDATE) {
      command.val[1] = 'S';
    } else {
      command.val[1] = 'P';
    }
  }
  
  void InfiniCommandMaker::insertLength(const BYTE len) {
    getThreeDigits(len, command.val, START_TOKEN_SZ);
  }
}
