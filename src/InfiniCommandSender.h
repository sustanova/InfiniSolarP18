#ifndef INFINI_COMMAND_SENDER
#define INFINI_COMMAND_SENDER

#include <Stream.h>

#include "InfiniCommandMaker.h"

namespace INFI {
  
  class InfiniCommandSender {
    public:

    //! This is the publicly accessible response msg.
    InfiniResponse response;
    
    /*! Constructs the command sender.
     * Note cmdStream is a ref while dbgStream is a pointer. It's a bit awkward.
     */
    InfiniCommandSender(Stream &cmdStream, Stream *dbgStream = NULL);

    /*! Makes and sends all the desired command chars over the command stream set in the ctor.
     */
    void sendCommand(COMMAND_TYPE commandType, const char* params);
  
    private:
    //! Print command's contents to stream as HEX. Assumes stream != NULL.
    void printCommandAsHex();
    
    InfiniCommandMaker m_cmdMaker;
    Stream &m_cmdStream;
    Stream *m_dbgStream;
  };
}
#endif
