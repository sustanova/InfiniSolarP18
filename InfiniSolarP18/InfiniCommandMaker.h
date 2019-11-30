#ifndef INFINI_COMMAND_MAKER
#define INFINI_COMMAND_MAKER

#include "InfiniMessageTypes.h"

namespace INFI {

  class InfiniCommandMaker {
    public:
    InfiniCommandMaker();
    
    //! This is the publicly accessible command msg.
    InfiniCommand command;

    //! Creates and inserts all the desired command chars.
    void makeCommand(COMMAND_TYPE commandType, const char* params);

    private:
    //! Helper to combine insertion of start token, length and command + param chars.
    void makeStartLengthCommand(COMMAND_TYPE commandType, const char* params);
    
    //! Updates length vars and inserts the start token, length, commandChars and params into command.
    void insertStartLengthCommand(ACTION_TYPE actionType, const char* commandChars, const char* params);
    
    //! Inserts '^P' or '^S' at beginning of commandMsg, depending on actionType.
    void insertStartAndType(ACTION_TYPE actionType);
  
    //! Insert len as a 3 digit number.
    void insertLength(const BYTE len);

    /*! 
     * The total length of the command, params, CRC and cr chars being currently considered.
     * This is equal to what is listed in the protocol manual.
     */
    BYTE m_cmdToEndSz;
  };
}

#endif
