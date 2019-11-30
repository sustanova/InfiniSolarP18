#include "InfiniCommandSender.h"

namespace INFI {
  
  InfiniCommandSender::InfiniCommandSender(Stream &cmdStream, Stream *dbgStream) :
    m_cmdStream(cmdStream),
    m_dbgStream(dbgStream)
  {}

  void InfiniCommandSender::sendCommand(COMMAND_TYPE commandType, const char* params) {
    // Reset the response buffer. Is this of any use?
    response.reset();

    // Set the cmdType to commandType
    response.cmdType = commandType;

    // Make the command
    m_cmdMaker.makeCommand(commandType, params);
    if (m_dbgStream != NULL) {
      printCommandAsHex();
    }
    
    // Send message to inverter!
    m_cmdStream.flush(); // What does this do? Why have I added it?
    m_cmdStream.write(m_cmdMaker.command.val, m_cmdMaker.command.actualLen);

    // Read the reply into response.
    // readBytesUntil is a blocking call, so that's probably bad for when we want to process
    // other things while we're collecting chars on Serial2 (which at 2400 baud is real slow).
    // See eg 3 here: https://forum.arduino.cc/index.php?topic=396450.0
    // It expects a start and end byte. We always know the start is '^' and the end is '\r'.
    response.actualLen = m_cmdStream.readBytesUntil('\r', response.val, response.bufferSize);
    
    // Add back the carriage return to last position.
    if (response.actualLen > 0) {
      if (response.actualLen < response.bufferSize) {
        response.val[response.actualLen] = '\r';
        response.actualLen++;
      } else if (m_dbgStream != NULL) {
        m_dbgStream->print("[InfiniCommandSender] response buffer too small, carriage return not appended.");
      }
    }
    
    if (m_dbgStream != NULL) {
      m_dbgStream->print("[InfiniCommandSender] ");
      m_dbgStream->println(response.val); 
      m_dbgStream->print("[InfiniCommandSender] Response had size "); m_dbgStream->println(response.actualLen);
    }
  }
  
  void InfiniCommandSender::printCommandAsHex() {
    m_dbgStream->print("[InfiniCommandSender] ");
    for (BYTE i = 0; i < m_cmdMaker.command.actualLen; ++i) {
      m_dbgStream->print(m_cmdMaker.command.val[i], HEX);
      m_dbgStream->print(" ");
    }
    m_dbgStream->println();
  }
}
